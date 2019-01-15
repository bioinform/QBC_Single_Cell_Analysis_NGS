#!/bin/bash
#========================================================================================
#Title:        submit_bash.sh
#Description:  This is a convenience script to run various step in the analysis pipeline.
#Author:       Sri R. Paladugu
#Date:         December 05 2018
#Version:      0.1
#Usage:        bash submit_bash.sh custom_job.txt
#Dependencies: Python-3.6.0, R-3.3.2, jdk8u92
#========================================================================================
set -e
set -u
set -o pipefail

# Proceed only if Python3 is installed
if ! command -v python3 &>/dev/null; then
    echo "Python 3 is not installed"
    exit 1
fi

# Proceed only if R is installed
if ! command -v Rscript &>/dev/null; then
    echo "R is not installed"
    exit 1
fi

# Proceed only if Python3 is installed
if ! command -v java &>/dev/null; then
    echo "Java is not installed"
    exit 1
fi

# Read custom_job.txt to fetch the variables and their values.
IFS="="
while read -r var_name var_value
do
    var_value="${var_value%%*( )}"   # delete trailing spaces
    var_value="${var_value%\"*}"     # delete opening quotes 
    var_value="${var_value#\"*}"     # delete closing quotes 
    declare $var_name=$var_value
    echo "Content of $var_name is $var_value"
done < $1

# Paths to various tools we use in our analysis pipeline.
parser="apps/parser/main"
rmsinglets_script="apps/remove_singlets/singlets_remover.py"
convert2fcs_script="apps/convert2fcs/generate_qbcReports.R"
normalization_jar="apps/normalization/QBCNormalizator-v004-alpha2.jar"

# Extract the sample name
sample_name=$(basename -- "$fasta_file")
sample_name=$(echo $sample_name | cut -d'.' -f1)
echo $sample_name

# Run the parser-step
echo "=== Running Parser Step ==="
parser_output="$output_folder/$sample_name/parser_output/"
mkdir -p $parser_output
$parser -f ${fasta_file} -i $qdata_folder -o $parser_output --save_bad_lines=1
rm ${parser_output}*.byCells

# Run remove-singlets step
echo "=== Running Remove Singlets Step ==="
by_fcs_file=$(find $parser_output -type f -name "*.byFCS")
rmsinglets_output="$output_folder/$sample_name/rmsinglets_output/"
mkdir -p $rmsinglets_output
python3 $rmsinglets_script --singlet_setting "$qdata_folder/Singlet_settings.txt" --by_FCS $by_fcs_file --outFolder $rmsinglets_output

# Run convert tab-delimited counts to fcs file step.
echo "=== Running conversion to fcs step ==="
parser_statistics_file=$(find $parser_output -type f -name "*.Statistics")
unjittered_forfcs_file=$(find $rmsinglets_output -type f -name "*.UNJITTERED_forFCS")
jittered_forfcs_file=$(find $rmsinglets_output -type f -name "*.JITTERED_forFCS")
fcs_output="$output_folder/$sample_name/fcs_output/"
mkdir -p $fcs_output
Rscript $convert2fcs_script --fcsFile ${unjittered_forfcs_file} --outFolder ${fcs_output}
Rscript $convert2fcs_script --fcsFile ${jittered_forfcs_file} --outFolder ${fcs_output}
rename '.UNJITTERED_forFCS' '_UNJITTERED' ${fcs_output}*UNJITTERED*.fcs
rename '.JITTERED_forFCS' '_JITTERED' ${fcs_output}*JITTERED*.fcs

# Run the normalization step.
echo "=== Running Normalization step ==="
fcs2_norm="$output_folder/$sample_name/norm_fcs_output/"
mkdir -p $fcs2_norm
mkdir -p "$fcs2_norm/tmp_fcs/"
unjittered_fcs_file=$(find $fcs_output -type f -name "*UNJITTERED*.fcs")
cp $unjittered_fcs_file "$fcs2_norm/tmp_fcs/"
java -jar $normalization_jar "$fcs2_norm/tmp_fcs/" $fcs2_norm 0 1 0.5
rm -rf "$fcs2_norm/tmp_fcs/"
rename '_UNJITTERED' '' ${fcs2_norm}*UNJITTERED*.fcs

#while IFS= read -r fasta_file
#do
#    printf '%s\n' "$fasta_file"
#done < ${fasta_files}
