#!/usr/bin/bash
#========================================================================================
#Title:        submit_bash.sh
#Description:  This is a convenience script to run various step in the analysis pipeline.
#Author:       Sri R. Paladugu
#Date:         May 03 2019
#Version:      v0.1
#Usage:        bash submit_bash.sh custom_job.txt
#Dependencies: Miniconda3, Python-3.6.0, R-3.3.2, jdk8u92
#========================================================================================
#set -e
#set -u
#set -o pipefail

# Proceed only if Miniconda3 is installed
if ! command -v conda &>/dev/null; then
    echo "Minconda/conda is not installed"
    exit 1
fi

# Install and Activate qbcCondaEnv
CONDA_BASE=$(conda info --base)
source ${CONDA_BASE}/etc/profile.d/conda.sh
bash qbc_conda_env.sh
conda activate qbcCondaEnv

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

# Proceed only if Java is installed
if ! command -v java &>/dev/null; then
    echo "Java is not installed"
    exit 1
fi

if [[ $# -eq 0 ]] ; then
    echo 'Please provide custom_job.txt as input.'
    exit 0
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
flash="apps/flash/flash"
seqtk="apps/seqtk/seqtk"
parser="apps/parser/build/main"
rmsinglets_script="apps/remove_singlets/singlets_remover.py"
convert2fcs_script="apps/convert2fcs/generate_qbcReports.R"
normalization_jar="apps/normalization/QBCNormalizator-v004-alpha2.jar"

# Extract the sample name
filename=$(basename "$read1")
IFS=' ' sarray=(${filename//_R1_/ })
sample_name=${sarray[0]}
echo $sample_name

# Run the flash-merge step
echo "=== Running FLASH merge Step ==="
flash_output="$output_folder/$sample_name/flash_output/"
mkdir -p $flash_output
$flash -d ${flash_output} -o ${sample_name} -M 150 $read1 $read2

# Run the seqtk step
echo "=== Running seqtk Step to convert fastq to fasta ==="
seqtk_output="$output_folder/$sample_name/seqtk_output/"
mkdir -p $seqtk_output
$seqtk seq -A ${flash_output}/${sample_name}.extendedFrags.fastq > ${seqtk_output}/${sample_name}.extendedFrags.fasta

# Run the parser-step
echo "=== Running Parser Step ==="
parser_output="$output_folder/$sample_name/parser_output/"
mkdir -p $parser_output
$parser -f ${seqtk_output}/${sample_name}.extendedFrags.fasta -i $qdata_folder -o $parser_output --save_bad_lines=1
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

echo "...The End..."
conda deactivate
