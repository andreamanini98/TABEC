#!/bin/zsh

# Shell script that performs a check on TA's emptiness as seen in theorem 5.

input_file="$1"
tChecker_liveness_path="$2"
# The integer magnification factor.
alpha_mag="$3"
output_tmp_file="$4"

# Check if the correct number of arguments is provided.
if [[ $# -ne 4 ]]; then
  echo "Error: arguments required: 3, arguments provided: $#."
  exit 1
fi

# Check if the input file exists.
if [[ ! -e "$input_file" ]]; then
  echo "Error: Input file does not exist."
  exit 1
fi

# Moving into the right folder for calling other scripts.
cd .. || exit
cd scriptsForChecks || exit

# Search for the pattern "# C :: int" in the input file.
C_pattern="# C :: [0-9]+"
# Search for the pattern "# alpha :: int" in the input file.
Alpha_pattern="# Alpha :: [0-9]+"
# The keyword representing the parameters to substitute.
param_keyword="param"

# Extract the values using grep, keeping only the last word of the result.
C=$(grep -oE "$C_pattern" "$input_file" | awk '{print $NF}')
Alpha=$(grep -oE "$Alpha_pattern" "$input_file" | awk '{print $NF}')

# Check if an integer was found.
if [[ -z "$C" ]] || [[ -z "$Alpha" ]]; then
  echo "Error: No integer found in the input file."
  exit 1
fi

loop_start=0
loop_end=$((4 * C))
# Variable used to tell at the end if the language is empty or not.
is_not_empty="false"

for n in $(seq "$loop_start" "$loop_end"); do
  # We calculate the new value of the parameter according to Theorem 5.
  mu=$(((n * alpha_mag / 2) + Alpha))
  # Replace the integer in the input file and save the result in the output file.
  sed "s/$param_keyword/$mu/g" "$input_file" > "$output_tmp_file"
  # Now calling tChecker to test the new substitution.
  result=$(./tCheckerLiveness.sh "$output_tmp_file" "$tChecker_liveness_path")
  # We check if the TA's language is empty
  if [[ "$result" == "true" ]]; then
    is_not_empty="true"
    break
  fi
done

# We echo the result since in the C++ caller function, it will be used to determine the checking result.
echo "$is_not_empty"
rm "$output_tmp_file"
