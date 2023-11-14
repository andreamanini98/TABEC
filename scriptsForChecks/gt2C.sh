#!/bin/zsh

# Shell script that, given an input file, substitutes all the occurrences of a parameter keyword with the appropriate
# integer value, producing the result in the file specified as output.

# WARNING: up to now, since this script is used to return either true or false based on the value contained in
#          the last line. For this reason, the last statement printed by the script must be a line where the
#          only word contained is either 'true' or 'false' based on the existence of an accepting condition or not.


# ----- PARAMETERS DEFINITIONS ----- #

# The path of the file to be used as input.
input_file="$1"

# The path of the file to be written with parameters keyword actually replaced by a value mu > 2C.
output_file="$2"

# The path to the tChecker's bin directory.
tChecker_liveness_path="$3"

# ----- PARAMETERS DEFINITIONS ----- #


# ----- VARIABLES DEFINITIONS ----- #

# Search for the pattern "# Q :: int" in the input file.
Q_pattern="# Q :: [0-9]+"

# Search for the pattern "# C :: int" in the input file.
C_pattern="# C :: [0-9]+"

# The keyword representing the parameters to substitute.
param_keyword="param"

# Check if the correct number of arguments is provided.
if [[ $# -ne 3 ]]; then
  echo "Error: arguments required: 3, arguments provided: $#."
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

# ----- VARIABLES DEFINITIONS ----- #


# ----- SCRIPT BODY ----- #

# Compute the value of the parameter we use for checking emptiness.
integer_gt_2C=$((1 + C * (1 + Q)))

# Replace the integer in the input file and save the result in the output file.
sed "s/$param_keyword/$integer_gt_2C/g" "$input_file" > "$output_file"

printf "Integer %s replaced and saved in:\n%s\n" "$integer_gt_2C" "$output_file"

# Moving into the right folder for calling other scripts.
cd ../scriptsForChecks || exit

# Now calling tChecker to test the emptiness.
result=$(./tCheckerLiveness.sh "$output_file" "$tChecker_liveness_path")

if [[ "$result" == "true" ]]; then
  printf "\nAcceptance condition found with parameter value: %s\n" "$integer_gt_2C"
  printf "In the original TA, this is equivalent to a parameter value of: %s\n" "$integer_gt_2C"
  is_not_empty="true"
else
  echo "Acceptance condition not found!"
  is_not_empty="false"
fi

# We print the result since in the C++ caller function, it will be used to determine the checking result.
printf "\nFinal result of (parameter < 2C) analysis: true == we have an accepting condition, false == the TA's language is empty.\n%s" "$is_not_empty"

# ----- SCRIPT BODY ----- #
