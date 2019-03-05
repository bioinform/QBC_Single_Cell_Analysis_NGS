# Quantum Barcoding : Ultra-High Throughput Single Cell Analysis of Proteins and RNAs by Split-pool Synthesis

Quantum Barcoding (QBC) is a method enabling simultaneous, ultra-high throughput single-cell-barcoding, of millions of cells for targeted single cell analysis of proteins and RNAs. This method circumvents the need to isolate single cells by building cell-specific oligo barcodes dynamically within each cell. Cell-specific codes are added to each tagged molecule within cells. 

This analysis workflow reads in a fasta file of a sequenced library built using of QBC_v1.0 barcodes, processes it through QBC-parse_v1.0 , filters and normalizes to generate a final output in FCS 4.0 standard Flow Cytometry format.  

QBC-parse_v1.0  - Deduplication based on unique molecular identifiers (UMI) – were performed using the QBC-parse_v1.0  which allows alignment of sequences with one mismatch. 

The QBC algorithm sequentially: a) detects barcode via alignments, b) corrects barcode by efficiently comparing barcodes to a whitelist, c) deduplicates based on UMI, d) evaluates reads for chimera filtering (check for evidence of PCR-based cross-over), e) filters reads for underrepresented/artificially created cells and  f)transforms sequences into table of cells and markers.

The data is further normalized as follows: expression values Ei,j for marker i in cell j were calculated by dividing unique read counts for marker i by the sum of the marker counts in cell j, to normalize for differences in coverage. The output is a corrected expression matrix which is then used as input for fcs conversion.

For more information contact us at bioinformatics.red@roche.com 



# QBC_Single_Cell_Analysis_NGS

## Description:
This repo contains a driver bash script to process the fasta files generated by QBC assay.
The output of the script is an fcs file which can be uploaded 
to standard flow-cytometry analysis software for further analysis.

## Initial Setup:

* Install necessary R packages:

``` bash
R
>source("https://bioconductor.org/biocLite.R")
>biocLite("flowCore")
>install.packages("optparse")
>install.packages("gridExtra")
>install.packages("ggplot2")
>if (!requireNamespace("BiocManager", quietly = TRUE))
>  install.packages("BiocManager")
>BiocManager::install("Biobase", version = "3.8")
>install.packages("scales")
>quit()
```

* Install necessary python modules:

``` bash
pip3 install --user pandas
pip3 install --user numpy
```

## Usage:

``` shell
git clone https://github.com/bioinform/QBC_Single_Cell_Analysis_NGS.git
cd QBC_Single_Cell_Analysis_NGS
```

Now using your favorite editor update the `custom_job.txt` file with the location of fasta file, qdata folder and output folder.

``` bash
nohup bash submit_bash.sh custom_job.txt &
```

## Output Folder Structure:

The bash script generates an output folder with the following top-level folder structure:

```bash
out_folder/Exp180G_S1_L001/
├── parser_output
│   ├── Exp180G_S1_L001_LStr_RC(-).bad
│   ├── Exp180G_S1_L001_LStr_RC(-).byFCS
│   ├── Exp180G_S1_L001_LStr_RC(-).crossoverFilter
│   ├── Exp180G_S1_L001_LStr_RC(-).junk
│   ├── Exp180G_S1_L001_LStr_RC(-).metrics
│   └── Exp180G_S1_L001_LStr.Statistics
├── rmsinglets_output
│   ├── Exp180G_S1_L001_LStr_RC(-)_10_5_5_1.JITTERED_forFCS
│   ├── Exp180G_S1_L001_LStr_RC(-)_10_5_5_1.tab_delimited
│   ├── Exp180G_S1_L001_LStr_RC(-)_10_5_5_1.UNJITTERED_forFCS
│   └── Summary.txt
├── fcs_output
│   ├── Exp180G_S1_L001_LStr_RC(-)_10_5_5_1_JITTERED.fcs
│   └── Exp180G_S1_L001_LStr_RC(-)_10_5_5_1_UNJITTERED.fcs
└── norm_fcs_output
    └── Exp180G_S1_L001_LStr_RC(-)_10_5_5_1_normalized_filtered_Chi2Pval_1.0_jittered0.5.fcs
```

### Files in `parser_output` folder
*  `Exp180G_S1_L001_LStr_RC(-).bad` - File containing reads where the parser encountered error in at least one sequence element (can be cell-barcodes or anchors or AHCA sequence).
*  `Exp180G_S1_L001_LStr_RC(-).byFCS` - A tab-delimited file with marker counts for each of the cells.
*  `Exp180G_S1_L001_LStr_RC(-).crossoverFilter` - File containing reads discarded by pcr-crossover filter.
*  `Exp180G_S1_L001_LStr_RC(-).junk` - File containing reads where the parser failed to detect all 7 barcode anchors.
*  `Exp180G_S1_L001_LStr_RC(-).metrics` - Metrics file containing reads counts filtered by each step in the parsing algorithm.
*  `Exp180G_S1_L001_LStr.Statistics` - A summary file containing statistics for cell-barcodes.

### Files in `rmsinglets_ouput` Folder
*  `Exp180G_S1_L001_LStr_RC(-)_10_5_5_1.tab_delimited` - A tab-delimited file containing cells and their corresponding marker counts.
*  `Exp180G_S1_L001_LStr_RC(-)_10_5_5_1.UNJITTERED_forFCS` - A tab-delimited file containing marker counts only (cell-identifier is discarded).
*  `Exp180G_S1_L001_LStr_RC(-)_10_5_5_1.JITTERED_forFCS` - A tab-delimited file containing jittered marker counts (cell-identifier column is dicarded).
*  `Summary.txt` - A summary file generated by remove-singlets script.

### Files in `fcs_output` Folder
*  `Exp180G_S1_L001_LStr_RC(-)_10_5_5_1_UNJITTERED.fcs` - Un-normalized fcs file containing marker counts for each of the cells.
*  `Exp180G_S1_L001_LStr_RC(-)_10_5_5_1_JITTERED.fcs` - Un-normalized fcs file containing jittered marker counts for each of the cells.

### Files in `norm_fcs_output` Folder
*  `Exp180G_S1_L001_LStr_RC(-)_10_5_5_1_forFCS_normalized_filtered_Chi2Pval_1.0_jittered0.5.fcs` - Normalized and Jittered fcs file. 
