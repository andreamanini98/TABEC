#!/bin/bash

# Shell script that, given an input file and a path to the tChecker liveness tool,
# checks if there exists a Büchi acceptance condition by printing true or false.

input_file="$1"
tChecker_liveness_path="$2"

"$tChecker_liveness_path" -a couvscc -l final "$input_file" | grep CYCLE | awk '{print $NF}'