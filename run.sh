#!/bin/bash

set -ev 

# Get the directory of the currently running script
script_dir=$(dirname "$0")

# Run the other script
"$script_dir/compile.sh"

./projeto_cad.exe ta80.jss parallel 1