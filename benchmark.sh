#!/bin/bash

set -e

# Get the directory of the currently running script
script_dir=$(dirname "$0")

# Run the other script
"$script_dir/compile.sh"

N_EXECUCOES=100

echo "gg03.jss sequential:"
./projeto_cad.exe gg03.jss sequential $N_EXECUCOES | grep Tempo 
echo "==================="

echo "gg03.jss parallel:"
./projeto_cad.exe gg03.jss parallel $N_EXECUCOES | grep Tempo
echo "==================="

echo "ff06.jss sequential:"
./projeto_cad.exe ff06.jss sequential $N_EXECUCOES | grep Tempo
echo "==================="

echo "ff06.jss parallel:"
./projeto_cad.exe ff06.jss parallel $N_EXECUCOES | grep Tempo
echo "==================="

echo "ta80.jss sequential:"
./projeto_cad.exe ta80.jss sequential $N_EXECUCOES | grep Tempo
echo "==================="

echo "ta80.jss parallel:"
./projeto_cad.exe ta80.jss parallel $N_EXECUCOES | grep Tempo
echo "==================="