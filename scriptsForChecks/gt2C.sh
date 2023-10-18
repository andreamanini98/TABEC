#!/bin/zsh

# Shell script that, given an input file, substitutes all the occurrences of a parameter keyword with the appropriate
# integer value, producing the result in the file specified as output.

input_file="$1"
output_file="$2"

# Search for the pattern "# Q :: int" in the input file.
Q_pattern="# Q :: [0-9]+"
# Search for the pattern "# C :: int" in the input file.
C_pattern="# C :: [0-9]+"
# The keyword representing the parameters to substitute.
param_keyword="param"

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

# Extract the values using grep, keeping only the last word of the result.
Q=$(grep -oE "$Q_pattern" "$input_file" | awk '{print $NF}')
C=$(grep -oE "$C_pattern" "$input_file" | awk '{print $NF}')

# Check if an integer was found.
if [[ -z "$Q" ]] || [[ -z "$C" ]]; then
  echo "Error: No integer found in the input file."
  exit 1
fi

# Compute the value of the parameter we use for checking emptiness.
integer_gt_2C=$((1 + C * (1 + Q)))

# Replace the integer in the input file and save the result in the output file.
sed "s/$param_keyword/$integer_gt_2C/g" "$input_file" > "$output_file"

echo "Integer $integer_gt_2C replaced and saved in $output_file."
