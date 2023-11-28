#!/bin/zsh

# Shell script that substitutes all the integers in a given file with a magnified version of such integers.
# This is done in order to get only integer constants to be used in the tools.


# ----- PARAMETERS DEFINITIONS ----- #

# The path of the file to be used as input.
input_file="$1"

# The path of the file to be used as output.
output_file="$2"

# The integer magnification factor.
alpha_mag="$3"

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

# ----- PARAMETERS DEFINITIONS ----- #


# ----- SCRIPT BODY ----- #

# Command used to substitute all integers appearing in guards and invariants with their magnified version.
sed -E "/^#/! s/( |^)([1-9][0-9]*)([ }]|$)/ \2$alpha_mag\3/g" "$input_file" > "$output_file"

# sed command explanation:
# ------------------------
# Start the 'sed' command with extended regular expressions enabled.
# Extended regular expressions allow for features like grouping and quantifiers.

#   sed -E

# Condition: If the line does not start with '#', apply the following substitution command:

#   "/^#/!

# Start of the substitution command.

#   s/

# A capturing group that matches either a blank space or the start of the line (^).

#   ( |^)

# A capturing group that matches an integer, starting with a non-zero digit (1-9)
# followed by zero or more other digits (0-9).

#   ([1-9][0-9]*)

# A capturing group that matches either a blank space, a closing curly brace '}', or the end of the line ($).

#   ([ }]|$)

# Separates the pattern from the replacement.

#   /

# The replacement pattern:
# - Adds a space before the original integer.
# - Appends $alpha_mag to the integer.
# - Uses '\2' to reference the second capturing group (the integer itself).
# - Uses '\3' to reference the third capturing group (blank space,
#   closing curly brace, or end of the line).

#   \2$alpha_mag\3

# The 'g' flag indicates that the substitution should be performed globally on each line.

#   /g

# Please remember that also the blank spaces in the sed regular expression matter!

# ----- SCRIPT BODY ----- #
