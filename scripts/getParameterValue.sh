#!/bin/zsh

# Shell script used to get the parameter value from the file specified in the 'input_file_path' parameter.


# ----- PARAMETERS DEFINITIONS ----- #

# The path of the file to be used as input.
input_file_path="$1"

# ----- PARAMETERS DEFINITIONS ----- #


# ----- VARIABLES DEFINITIONS ----- #

# Check if the correct number of arguments is provided.
if [[ $# -ne 1 ]]; then
  echo "Error: arguments required: 1, arguments provided: $#."
  exit 1
fi

# ----- VARIABLES DEFINITIONS ----- #


# ----- SCRIPT BODY ----- #

# Fetching the value of the parameter.
# Please note that this will return only one possible parameter.
# The final sed is used to transform the comma in the floating point number into a dot.
param_value=$(grep -A 2 "[[ ACC ]]" "$input_file_path" | tail -n 1 | awk '{print $NF}' | sed 's/,/./')

echo "$param_value"

# ----- SCRIPT BODY ----- #
