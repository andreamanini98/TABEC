#!/bin/zsh

# Shell script that, given an input file and a path to the tChecker liveness tool,
# checks if there exists a Büchi acceptance condition by printing true or false.


# ----- PARAMETERS DEFINITIONS ----- #

# The path to the input file we want to give to tChecker.
input_file="$1"

# The path to the tChecker liveness tool.
tChecker_liveness_path="$2"

# Check if the correct number of arguments is provided.
if [[ $# -ne 2 ]]; then
  echo "Error: arguments required: 2, arguments provided: $#."
  exit 1
fi

# Check if the input file exists.
if [[ ! -e "$input_file" ]]; then
  echo "Error: Input file does not exist."
  exit 1
fi

# ----- PARAMETERS DEFINITIONS ----- #


# ----- SCRIPT BODY ----- #

$tChecker_liveness_path -a couvscc -l final "$input_file" | grep CYCLE | awk '{print $NF}'

# ----- SCRIPT BODY ----- #
