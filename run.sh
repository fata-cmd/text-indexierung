#!/bin/bash

# Array of types
types=("Variable" "HashMap" "VariableSIMD")

# First argument as the words file
words_file=$1

# Check if the words file argument is provided
if [[ -z $words_file ]]; then
  echo "Usage: $0 <words_file>"
  exit 1
fi

# Loop through each type and process with the build/main command
for type in "${types[@]}"; do
  for i in {1..1}; do
    echo "Processing: $type"
  
    # Construct the query file path by concatenating the type with the words file
    query_file="queries_${words_file}"
  
    # Execute the command with the given words file and constructed query file
    echo -e "$type:" >> results.txt
    ./build/main "$words_file" "$query_file" "$type" >> results.txt
    
  
    # Clear the cache
    sudo sync
    sudo sh -c "echo 3 > /proc/sys/vm/drop_caches"
    
    echo "Cache cleared after processing: $type"
  done
done
