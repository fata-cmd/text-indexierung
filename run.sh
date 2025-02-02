#!/bin/bash

CORE="0,1,2,3"

# Array of types
types=("1" "2" "3" "4" "5")
input_dir="./resources/input"
queries_dir="./resources/queries"
output_dir="./resources/output"

num_reps=3
file_count=$(find "$input_dir" -maxdepth 1 -type f | wc -l)
num_types=${#types[@]}

num_runs=$((num_reps * file_count * num_types))
run_counter=1

# Loop through all files in the input directory
for words_file in "$input_dir"/*; do

  # Check if the words file argument is provided
  if [[ ! -f "$words_file" ]]; then
      echo "$words_file not found"
      continue
  fi
  echo "Processing: $words_file"	
  filename=$(basename "$words_file")
  output_file="${output_dir}/${filename}"

  if [ ! -f "${output_file}" ]; then
    touch "${output_file}"
  fi

  # Loop through each type and process with the build/main command
  for type in "${types[@]}"; do
    for i in $(seq 1 "$num_reps"); do
      echo "Run ${run_counter} out of ${num_runs}"
      ((run_counter++))
      # Construct the query file path by concatenating the type with the words file
      query_file="${queries_dir}/${filename}"
    
      # Execute the command with the given words file and constructed query file
      taskset -c $CORE sudo chrt -f 99 ./build/main "$type" "$words_file" "$query_file" >> "${output_file}"
      
    
      # Clear the cache
      sudo sync
      sudo sh -c "echo 3 > /proc/sys/vm/drop_caches"
      
    done
  done
done
