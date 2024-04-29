#!/bin/bash

set -e

# Get the directory of the currently running script
script_dir=$(dirname "$0")

# Run the other script
"$script_dir/compile.sh"

echo -n "gg03.jss sequential:"
./projeto_cad.exe gg03.jss sequential | grep Tempo | grep -o -E "([0-9]+\.[0-9]+)"

echo -n "gg03.jss parallel:"
./projeto_cad.exe gg03.jss parallel | grep Tempo | grep -o -E "([0-9]+\.[0-9]+)"

echo -n "ff06.jss sequential:"
./projeto_cad.exe ff06.jss sequential | grep Tempo | grep -o -E "([0-9]+\.[0-9]+)"

echo -n "ff06.jss parallel:"
./projeto_cad.exe ff06.jss parallel | grep Tempo | grep -o -E "([0-9]+\.[0-9]+)"