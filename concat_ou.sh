#!/bin/bash

# Define the folder path
folder_path="./resources/output"
summary_file="$folder_path/summary.csv"

# Create or clear the summary file
> "$summary_file"

# Loop through each CSV file in the folder
for csv_file in "$folder_path"/*.txt; do
  # Extract the filename without extension
  filename=$(basename "$csv_file" .txt)
  
  # Read each line in the CSV file
  while IFS= read -r line || [ -n "$line" ]; do
    # Append to summary.csv with filename as first column
    echo "$filename,$line" >> "$summary_file"
  done < "$csv_file"
done

echo "CSV files merged into $summary_file successfully!"
