#!/usr/bin/bash

CONDA_NAME="qbcCondaEnv"
CONDA_YML="environment.yml"

CONDA_PATH="$(conda env list|grep $CONDA_NAME | awk '{print $2}')"

# Activate conda environment.
if [[ ! -z $CONDA_PATH ]];then
        echo "Updating Conda environment."
        conda env update -f $CONDA_YML
        touch $CONDA_PATH
else
    echo "Environment $CONDA_NAME not detected. Creating Conda environment."
    conda env create -f $CONDA_YML
fi
