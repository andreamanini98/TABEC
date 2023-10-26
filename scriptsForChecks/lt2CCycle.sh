#!/bin/zsh

# Shell script that performs a check on TA's emptiness, first trying all parameter values multiple
# of 0.5 and, in case a solution is not found, uses the alpha value to perform a check as seen in Theorem 5.

# The path of the file to be used as input.
input_file="$1"
# The path to the tChecker's bin directory.
tChecker_liveness_path="$2"
# The integer magnification factor.
alpha_mag="$3"
# Path where a temporary file will be created for performing checks below.
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

# Variable used to tell if a TA's language is empty or not.
is_not_empty="false"


# ----- Testing parameter multiple of 0.5 and less than or equal to 2C. ----- #

echo "Now starting testing parameter values multiple of 0.5 and less than or equal to 2C"

loop_start=0
loop_end=$((4 * C - 1)) # This ensures to try all multiples of 0.5 if we set mu as below.

echo "Entering cycle: start = $loop_start, end = $loop_end"

for n in $(seq "$loop_start" "$loop_end"); do
  # We calculate the new value of the parameter according to Theorem 5.
  printf "\nStarting iteration number: %s\n" "$n"

  # Here we start from 0.5. However, we have to magnify it (as done for all the other constants in the TA).
  # Please note that we have 5 and a division by 10 here: this is to obtain the same result of putting 0.5 without any division.
  # This has been done since a shell script may not support decimals or floating point numbers.
  mu=$((n * alpha_mag / 2 + (5 * alpha_mag / 10)))

  echo "Now trying parameter value: $mu"

  # Replace the integer in the input file and save the result in the temporary output file.
  sed "s/$param_keyword/$mu/g" "$input_file" > "$output_tmp_file"

  # Now calling tChecker to test the new substitution.
  result=$(./tCheckerLiveness.sh "$output_tmp_file" "$tChecker_liveness_path")

  # We check if the TA's language is empty or not.
  if [[ "$result" == "true" ]]; then
    echo "Solution found!"
    is_not_empty="true"
    break
  else
    echo "Solution not found, starting new loop iteration"
  fi
done


if [[ "$is_not_empty" == "true" ]]; then
  printf "\nAcceptance condition found with parameter value: %s\n" "$mu"
  printf "In the original TA, this is equivalent to a parameter value of: (%s/%s)\n" "$mu" "$alpha_mag"
else


  # ----- Testing parameter of the form: (n / 2) + alpha. ----- #

  printf "\nAcceptance condition not found, we now have to try parameter values of the form: (n / 2) + alpha\n"

  loop_start=0
  loop_end=$((4 * C - 1))

  echo "Entering cycle: start = $loop_start, end = $loop_end"

  printf "\nStarting loop to check parameter values of the form: (n / 2) + alpha\n"
  printf "Alpha value: %s\n" "$Alpha"

  for n in $(seq "$loop_start" "$loop_end"); do
    # We calculate the new value of the parameter according to Theorem 5.
    printf "\nStarting iteration number: %s\n" "$n"

    mu=$(((n * alpha_mag / 2) + Alpha))

    echo "Now trying parameter value: $mu"

    # Replace the integer in the input file and save the result in the temporary output file.
    sed "s/$param_keyword/$mu/g" "$input_file" > "$output_tmp_file"

    # Now calling tChecker to test the new substitution.
    result=$(./tCheckerLiveness.sh "$output_tmp_file" "$tChecker_liveness_path")

    # We check if the TA's language is empty or not.
    if [[ "$result" == "true" ]]; then
      echo "Solution found!"
      is_not_empty="true"
      break
    else
      echo "Solution not found, starting new loop iteration"
    fi
  done


  if [[ "$is_not_empty" == "true" ]]; then
    printf "\nAcceptance condition found with parameter value: %s\n" "$mu"
    printf "In the original TA, this is equivalent to a parameter value of: (%s/%s)\n" "$mu" "$alpha_mag"
  else
    echo "Acceptance condition not found"
  fi


fi

# We print the result since in the C++ caller function, it will be used to determine the checking result.
printf "\nFinal result of analysis: is the language of the given TA not empty?\n%s" "$is_not_empty"
rm "$output_tmp_file"
