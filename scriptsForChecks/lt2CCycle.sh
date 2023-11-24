#!/bin/bash

# Shell script that performs a check on TA's emptiness, first trying all parameter values multiple
# of 0.5 and, in case a solution is not found, uses the alpha value to perform a check as seen in Theorem 5.

# WARNING: up to now, since this script is used to return either true or false based on the value contained in
#          the last line. For this reason, the last statement printed by the script must be a line where the
#          only word contained is either 'true' or 'false' based on the existence of an accepting condition or not.


# ----- PARAMETERS DEFINITIONS ----- #

# The path of the file to be used as input.
input_file="$1"

# The path to the tChecker's bin directory.
tChecker_liveness_path="$2"

# The integer magnification factor.
alpha_mag="$3"

# Path where a temporary file will be created for performing checks below.
output_tmp_file="$4"

# Used to either log all values of the parameters (all) or stop at the first acceptance condition found (lla).
log_all="$5"

# ----- PARAMETERS DEFINITIONS ----- #


# ----- VARIABLES DEFINITIONS ----- #

# Check if the correct number of arguments is provided.
if [[ $# -ne 5 ]]; then
  echo "Error: arguments required: 5, arguments provided: $#."
  exit 1
fi

# Check if the input file exists.
if [[ ! -e "$input_file" ]]; then
  echo "Error: Input file does not exist."
  exit 1
fi

# Moving into the right folder for calling other scripts.
cd ../scriptsForChecks || exit

# Search for the pattern "# C :: int" in the input file.
C_pattern="# C :: [0-9]+"

# Search for the pattern "# alpha :: int" in the input file.
Alpha_pattern="# Alpha :: [0-9]+"

# The keyword representing the parameters to substitute.
param_keyword="param"

# Extracting the values using grep, keeping only the last word of the result.
C=$(grep -oE "$C_pattern" "$input_file" | awk '{print $NF}')
Alpha=$(grep -oE "$Alpha_pattern" "$input_file" | awk '{print $NF}')

# Check if an integer was found.
if [[ -z "$C" ]] || [[ -z "$Alpha" ]]; then
  echo "Error: No integer found in the input file."
  exit 1
fi

# Variable used to tell if a TA's language is empty or not.
is_not_empty="false"

# ----- VARIABLES DEFINITIONS ----- #


# ----- FUNCTIONS DEFINITIONS ----- #

# Function used to print information about the final value of the parameter, if an accepting condition has been found.
# Parameters:
# $1 : the value of the parameter to be scaled.
print_scaled_param() {
  local mu="$1"
  printf "[[ ACC ]]\nAcceptance condition found with parameter value: %s\n" "$mu"

  # Getting the number of digits inside alpha_mag in order to have the correct amount of decimal digits in the output.
  local alpha_chars="${#alpha_mag}"

  # Computing and printing the decimal value of the parameter.
  local scaled_param
  scaled_param="$(bc <<< "scale=$alpha_chars; $mu / $alpha_mag")"
  printf "In the original TA, this is equivalent to a parameter value of: %.${alpha_chars}f\n" "$scaled_param"
}

# Function used to check the emptiness of the TA by repeatedly changing the
# value of the parameter according to an appropriate parameter update policy.
# Parameters:
# $1 : an integer telling the function how to update mu during the loop.
check_emptiness() {
  local mu_selector="$1"

  local loop_start=0
  local loop_end=$((4 * C - 1)) # This ensures to try all multiples of 0.5 if we set mu as below.

  echo "Entering cycle: start = $loop_start, end = $loop_end."

  local n
  for n in $(seq "$loop_start" "$loop_end"); do
    # We calculate the new value of the parameter according to Theorem 5.
    printf "\nStarting iteration number: %s\n" "$n"

    case "$mu_selector" in
      0)
        # Here we start from 0.5. However, we have to magnify it (as done for all the other constants in the TA).
        # Please note that we have 5 and a division by 10 here: this is to obtain the same result of putting 0.5 without any division.
        # This has been done since a shell script may not support decimals or floating point numbers.
        mu=$((n * alpha_mag / 2 + (5 * alpha_mag / 10)))
        ;;
      1)
        mu=$(((n * alpha_mag / 2) + Alpha))
        ;;
    esac

    echo "Now trying parameter value: $mu"

    # Replace the integer in the input file and save the result in the temporary output file.
    sed "s/$param_keyword/$mu/g" "$input_file" > "$output_tmp_file"

    # Now calling tChecker to test the new substitution.
    local result
    result=$(./tCheckerLiveness.sh "$output_tmp_file" "$tChecker_liveness_path")

    # We check if the TA's language is empty or not.
    if [[ "$result" == "true" ]]; then
      print_scaled_param "$mu"
      is_not_empty="true"

      if [[ "$log_all" == "lla" ]]; then
        break
      fi
    else
      echo "Solution not found with parameter value: $mu, starting new loop iteration."
    fi
  done
}

# ----- FUNCTIONS DEFINITIONS ----- #


# ----- SCRIPT BODY ----- #

echo "Now starting testing parameter values multiple of 0.5 and less than or equal to 2C."

# Testing parameter multiple of 0.5 and less than or equal to 2C.
check_emptiness 0

if [[ "$is_not_empty" != "true" ]]; then
  printf "\nAcceptance condition not found, we now have to try parameter values of the form: (n / 2) + alpha.\n"
  printf "\nStarting loop to check parameter values of the form: (n / 2) + alpha.\n"
  printf "Alpha value: %s\n" "$Alpha"

  # Testing parameter of the form: (n / 2) + alpha.
  check_emptiness 1

  if [[ ! "$is_not_empty" == "true" ]]; then
    echo "Acceptance condition not found."
  fi
fi

# We print the result since in the C++ caller function, it will be used to determine the checking result.
printf "\nFinal result of (parameter < 2C) analysis: true == we have an accepting condition, false == the TA's language is empty.\n%s" "$is_not_empty"

# Finally, we remove the temporary created file.
rm "$output_tmp_file"

# ----- SCRIPT BODY ----- #
