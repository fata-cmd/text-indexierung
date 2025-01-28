#!/bin/bash

# Input directory containing word files
input_dir="./resources/input"

# Loop through all files in the input directory
for words_file in "$input_dir"/*; do

    # Skip if it's not a file
    if [[ ! -f "$words_file" ]]; then
        continue
    fi

    # Run ./build/gen and capture the filename it outputs
    queries_file=$(./build/gen "$words_file")

    # Check if the file exists
    if [[ ! -f "$queries_file" ]]; then
        echo "Error: Generated file '$queries_file' does not exist."
        exit 1
    fi

    # Shuffle the contents and write to a temporary file
    temp_file=$(mktemp)

    trap "rm -f $temp_file" EXIT

    shuf "$queries_file" -o "$temp_file"

    # Move the temporary file back to the original file
    mv "$temp_file" "$queries_file"

    echo "Queries file shuffled and stored at: $queries_file"
done