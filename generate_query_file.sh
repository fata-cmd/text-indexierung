#!/bin/bash

words_file=$1

./build/gen "$words_file"

# Shuffle the contents and write to a temporary file
temp_file=$(mktemp)
shuf "$words_file" -o "$temp_file"

# Move the temporary file back to the original file
mv "$temp_file" "$words_file"