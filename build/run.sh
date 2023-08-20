#!/bin/bash
suffix=".lasm"

rm examples/*.lexe
make -B
for file in examples/*"$suffix"; do
    # Check if the file exists and is a regular file
    if [ -f "$file" ]; then
        # Extract the file name without the suffix
        filename_no_suffix="${file%$suffix}"
        ./lasm -i "$file" -o "$filename_no_suffix.lexe"
        if [ -e "$filename_no_suffix.lexe" ]; then
            ./lexe "$filename_no_suffix.lexe"
        else
            echo "Failed to create $filename_no_suffix.lexe"
        fi
    fi
done