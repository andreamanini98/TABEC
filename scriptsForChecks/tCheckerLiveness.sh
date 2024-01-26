#!/bin/zsh

# Shell script that, given an input file and a path to the tChecker liveness tool,
# checks if there exists a Büchi acceptance condition by printing true or false.


# ----- PARAMETERS DEFINITIONS ----- #

# The path to the input file we want to give to tChecker.
input_file="$1"

# The path to the tChecker liveness tool.
tChecker_liveness_path="$2"

# The name of the file .txt file in which to store resource usage of the TA under analysis.
ta_results_file_name="$3"

# Path to the directory containing the files used for storing resource usages.
testing_resource_usage_directory="$4"

# Check if the correct number of arguments is provided.
if [[ $# -ne 4 ]]; then
  echo "Error: arguments required: 4, arguments provided: $#."
  exit 1
fi

# Check if the input file exists.
if [[ ! -e "$input_file" ]]; then
  echo "Error: Input file does not exist."
  exit 1
fi

# ----- PARAMETERS DEFINITIONS ----- #


# ----- SCRIPT BODY ----- #

# MEMORY-MAX_RSS is measured in Bytes.

# Getting the full output from the execution of tChecker.
res=$($tChecker_liveness_path -a couvscc -l final "$input_file")

# Printing the full output into the file collecting information about resource usage.
printf "$res\n\n" >> "$testing_resource_usage_directory/$ta_results_file_name"

# Echoing the last word taken from the line containing the word 'CYCLE' that will be used to determine if the language is empty or not.
echo "$res" | grep CYCLE | awk '{print $NF}'

# ----- SCRIPT BODY ----- #
