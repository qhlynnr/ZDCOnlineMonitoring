#!/bin/bash

# Loop through each line of the input_tags.txt file
while read -r column1 column2 column3; do
    # Run the makeDump_Run3.csh script with the columns as arguments
    ./makeDump_Run3.csh "$column1" "$column2" "$column3"
done < input_tags_2026_PbPb.txt
