#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cmake -DCMAKE_BUILD_TYPE=Release .
make

movefiles(){
    for file in "$1"/* 
    do 
    if [ -d "$file" ]
    then 
            movefiles "$file"
    else
        if [ -x "$file" ]; then
            if [[ $file != *.txt ]] && [[ $file != *.cpp ]] && [[ $file != *.cmake ]] && [[ $file != *.o ]] && [[ $file != *.make ]] && [[ $file != *.d ]] && [[ $file != *.ts ]] && [[ $file != *.marks ]] && [[ $file != "Makefile" ]]
            then
                mv "$file" "${SCRIPT_DIR}/build"
            fi
        fi
    fi
    done
}

removefiles(){
    for file in "$1"/* 
    do 
    if [ -d "$file" ]
    then 
            if [ "$file" = "CMakeFiles" ]
            then
                rm -r "$file"
            fi
    else
        if [ -x "$file" ]; then
            if [[ $file == *.cmake ]] && [[ $file == *.o ]] && [[ $file == *.make ]] && [[ $file == *.d ]] && [[ $file == *.ts ]] && [[ $file == *.marks ]] && [[ "$file" != "Makefile" ]]
            then
                rm "$file"
            fi
        fi
    fi
    done
}

movefiles "$1"
removefiles "$1"

cd "${SCRIPT_DIR}/build"
# ./to_polygon -i ../instances/instances/data/images/london-0000010.instance -o results-euro-night-0000015.txt -algorithm local_search -edge_selection 3 -initialization 1a
./to_polygon -i ../instances/test.txt -o results-euro-night-0000015.txt -algorithm local_search -edge_selection 3 -initialization 1a

