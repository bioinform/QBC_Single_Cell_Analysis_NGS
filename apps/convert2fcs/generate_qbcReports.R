#!/usr/bin/env Rscript
library("optparse")
library("flowCore")
library("Biobase")

options(warn=-1)
## usage Rscript convert2fcs --fcsFile $forfcsfile --statsFile $statisticsFile

option_list = list(
  make_option( c("-fcs", "--fcsFile"), type = "character", default = NULL,
               help = "file containing tsv data", metavar = "character"),
  make_option(c("-stats", "--statsFile"), type = "character", default = NULL,
              help = "file containing statistics data for cuda-parse", metavar = "character"),
  make_option(c("-out", "--outFolder"), type = "character", default = NULL,
              help = "output folder path", metavar = "character")
)

opt_parser = OptionParser(option_list=option_list);
opt = parse_args(opt_parser);
if (is.null(opt$fcsFile) & is.null(opt$statsFile)){
  print_help(opt_parser)
  stop("At least one argument must be supplied (input file).n", call.=FALSE)
}

# read in ._forFCS file output from rmsinglets module
getFCSData <- function(filename_in) {
  if (!is.character(filename_in) || length(filename_in) != 1) 
    stop("'filename' must be character scalar")
  if (!file.exists(filename_in)) 
    stop(paste("'", filename_in, "' is not a valid file", sep = ""))
  
  x <- read.table(filename_in,sep="\t",header=TRUE,stringsAsFactors=FALSE)
  x[] <- lapply(x, as.numeric)
  return(x)
}

# prepares and writes out a tsv file that is consumable by commercial 
# FCS analysis tools likeCytoBank and flowJo
tsv2fcs <- function(xData) {
  
  filename_out = paste(opt$outFolder, tools::file_path_sans_ext( basename( opt$fcsFile ) ), ".fcs", sep = "")

  if (colnames(xData)[1] == 'Barcode') {
    xData <- xData[, 2:ncol(xData)] #delete barcode column
  }
  
  #create event number column (can be used to trace back to barcode later)
  eventnum <- matrix(1:nrow(xData))
  xData <- cbind(eventnum, xData)
  colnames(xData)[1] <- "eventnum"
  
  #create fcs header
  params <- c()
  descrip <- list()
  
  descrip[["$DATATYPE"]] <- "F"
  descrip[["$MODE"]] <- "L"

  for (i in 1:ncol(xData)) {
    maxi <- max(xData[, i])
    rng <- maxi + 1
    
    pl <- matrix(c(colnames(xData)[i], colnames(xData)[i], rng, 0, maxi), nrow = 1)
    colnames(pl) <- c("name", "desc", "range", "minRange", "maxRange")
    rownames(pl) <- paste("$P", i, sep = "")
    params <- rbind(params, pl)
    
    descrip[[paste("$P", i, "B", sep = "")]] <- "32" # Number of bits
    descrip[[paste("$P", i, "R", sep = "")]] <- toString(rng) # Range
    descrip[[paste("$P", i, "E", sep = "")]] <- "0,0" # Exponent
    descrip[[paste("$P", i, "N", sep = "")]] <- colnames(xData)[i] # name
    descrip[[paste("$P", i, "S", sep = "")]] <- colnames(xData)[i] # name2
  }
  
  y <- flowFrame(as.matrix(xData), as(data.frame(params), "AnnotatedDataFrame"), description = descrip)

  print(paste("Creating file",filename_out,sep=" "))
  suppressWarnings(write.FCS(y, filename_out, "numeric"))
  
}
