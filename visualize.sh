#!/bin/sh

for words_file in ./resources/output/*; do
    python3 "./visualize.py" "$words_file"
done