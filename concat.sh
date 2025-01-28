#!/bin/bash

# Define the Maildir path and the output file
MAILDIR_PATH="maildir"   # Replace with the actual path to your Maildir
OUTPUT_FILE="all_documents.txt"      # Name of the output file where all files will be concatenated

# Create or clear the output file
> "$OUTPUT_FILE"

# Function to recursively go through all folders and concatenate files
concatenate_files() {
  local dir="$1"

  # Loop through all files and directories in the current directory
  for file in "$dir"/*; do
    if [ -d "$file" ]; then
      # If it's a directory, recursively call the function
      concatenate_files "$file"
    elif [ -f "$file" ]; then
      # If it's a regular file, append its contents to the output file
      cat "$file" >> "$OUTPUT_FILE"
    fi
  done
}

# Start the concatenation process from the Maildir
concatenate_files "$MAILDIR_PATH"

echo "All files have been concatenated into $OUTPUT_FILE"
