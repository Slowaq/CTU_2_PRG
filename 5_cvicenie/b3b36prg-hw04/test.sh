#!/bin/bash

#example run:
#   bash test.sh executable_name[optional]
#   if executable is not given it finds by 





# Get the current directory
current_dir=$(pwd)
tests_folder="$current_dir"/data/man

# Loop through all files with .in suffix in the current directory
for in_file in "$tests_folder"/*.in; do
  if [[ -f "$in_file" ]]; then
    # Extract the filename without the .in suffix
    filename=$(basename "$in_file" .in)

    in=$in_file
    out="$tests_folder/$filename.out"
    out="$tests_folder/$filename.out"

    mkdir -p ./test

    if [ -z "$1" ]; then
        executable_name=hw*prg
    else
        executable_name=$1
    fi
   

    ./$executable_name > test/$filename.out 2> test/$filename.err < $in_file

    echo "diff test/$filename.out data/$filename.out"
    diff test/$filename.out data/man/$filename.out
    
    
    echo "diff test/$filename.err data/$filename.err"
    diff test/$filename.err data/man/$filename.err

  else
    echo "No .in files found in the current directory."
    break # or exit 1
  fi
done