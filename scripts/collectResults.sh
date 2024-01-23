#!/bin/zsh

# Shell script used to run the checker and subsequently collect the results from the produced files,
# conveniently rewriting them in a single file in which the following information are specified:
# - The name of the TA.
# - An alert telling if the language is empty or not. If the language is empty, also a possible value for the parameter is reported.
# - The tiles used for the TA construction. Useful to have an idea of the values range of the parameter.


# ----- PARAMETERS DEFINITIONS ----- #

# The path of the directory to be used as input.
input_dir_path="$1"

# The path of the directory to be used as output.
output_dir_path="$2"

# The path to the tChecker's bin folder.
tChecker_bin_path="$3"

# Parameter used to tell if the checker has to be run automatically after the test generation.
automatic_run_checker="$4"

# Parameter used to automatically print the results at the end of the checker's execution.
automatic_print_result="$5"

# ----- PARAMETERS DEFINITIONS ----- #


# ----- VARIABLES DEFINITIONS ----- #

# Check if the correct number of arguments is provided.
if [[ $# -ne 5 ]]; then
  echo "Error: arguments required: 5, arguments provided: $#."
  exit 1
fi

# Moving into the right folder for calling executables.
cd ../executables || exit

# The name of the file in which results will be collected.
output_file_name="Results.txt"

# ----- VARIABLES DEFINITIONS ----- #


# ----- SCRIPT BODY ----- #

if [[ "$automatic_run_checker" == "1" ]]; then

  # Calling the checker executable in order to perform the emptiness checking on the TAs generated by the tester.
  ./checker -lns "$tChecker_bin_path" -all

  for f in "$input_dir_path"/*; do
    # The name of the TA corresponds to the name of the file (full path excluded).
    TA_name=$(basename "$f")

    # Fetching the name of the tiles used to generate the TA.
    used_tiles=$(grep -A 2 "Used tiles (random and accepting tiles excluded):" "$f" | tail -n +2)

    # Fetching the value of the parameter.
    # Please note that this will return only one possible parameter.
    # The final sed is used to transform the comma in the floating point number into a dot.
    param_value=$(grep -A 2 "[[ ACC ]]" "$f" | tail -n 1 | awk '{print $NF}' | sed 's/,/./')

    # String used to inform if the TA's language is empty or not.
    empty_language_alert="Language is empty"

    # Getting the last word of the file telling if the language is empty or not.
    isNotEmpty=$(tail -n 1 "$f")

    if [[ "$isNotEmpty" == "true" ]]; then
      empty_language_alert="Language is not empty with parameter value: $param_value"
    fi

    { echo "$TA_name";
      echo "$empty_language_alert";
      echo "Tiles used: $used_tiles";
      printf "\n\n" } >> "$output_dir_path/$output_file_name"
  done

  if [[ "$automatic_print_result" == "1" ]]; then
    clear
    cat "$output_dir_path/$output_file_name"
  fi

 fi

# ----- SCRIPT BODY ----- #
