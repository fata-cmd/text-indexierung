#!/bin/bash

# Check if a filename was provided as an argument
if [ $# -ne 1 ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

# Get the filename
filename=$1

# Extract unique words, convert to lowercase, remove non-alphanumeric characters and sort them
unique_words=$(cat "$filename" | tr -cs '[:alnum:]' '[\n*]' | sort | uniq)

# Count unique words
num_unique_words=$(echo "$unique_words" | wc -l)

# Calculate the total sum of word lengths
total_length=0
while read -r word; do
    total_length=$((total_length + ${#word}))
done <<< "$unique_words"

# Calculate the average word length
if [ $num_unique_words -gt 0 ]; then
    avg_length=$((total_length / num_unique_words))
else
    avg_length=0
fi

# Calculate overall similarity (Jaccard similarity between all unique word pairs)
total_similarity=0
word_array=()
index=0
while read -r word; do
    word_array[$index]=$word
    index=$((index + 1))
done <<< "$unique_words"

for ((i=0; i<$num_unique_words; i++)); do
    for ((j=i+1; j<$num_unique_words; j++)); do
        # Get the words
        word1=${word_array[$i]}
        word2=${word_array[$j]}
        
        # Calculate Jaccard similarity between the two words (intersection of unique characters)
        intersection=$(echo "$word1$word2" | fold -w1 | sort | uniq -d | wc -l)
        union=$(echo "$word1$word2" | fold -w1 | sort | uniq | wc -l)
        similarity=$(echo "scale=2; $intersection / $union" | bc)
        
        # Add to the total similarity
        total_similarity=$(echo "$total_similarity + $similarity" | bc)
    done
done

# Calculate the average similarity
if [ $num_unique_words -gt 1 ]; then
    avg_similarity=$(echo "scale=2; $total_similarity / ($num_unique_words * ($num_unique_words - 1) / 2)" | bc)
else
    avg_similarity=0
fi

# Print the results
echo "Number of unique words: $num_unique_words"
echo "Total sum of word lengths: $total_length"
echo "Average word length: $avg_length"
echo "Total similarity (sum of pairwise Jaccard similarities): $total_similarity"
echo "Average similarity: $avg_similarity"
