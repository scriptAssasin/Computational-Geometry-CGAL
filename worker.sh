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

#./optimal_polygon -i ../instances/instances/data/images/london-0000015.instance -o results-antColony-0000015.txt -algorithm ant_colony -L 20 -max -alpha 0.5 -beta 0.2 -ro 0.3 -elitism 1
#./optimal_polygon -i ../instances/instances/data/images/london-0000015.instance -o results-local_search-0000015.txt -algorithm local_search -min -threshold 2.0 
#./optimal_polygon -i ../instances/instances/data/images/london-0000015.instance -o results-simulated_annealing-0000015.txt -algorithm simulated_annealing -max -L 5000 -annealing local 
