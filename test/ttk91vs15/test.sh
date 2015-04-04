#!/bin/bash

cd "$(dirname "$0")"

target="../../target/"
compiler=$target"compiler"
linker=$target"linker"
machine=$target"ttk-15"

red='\033[0;31m'
green='\033[0;32m'
NC='\033[0m'

echo " "
echo "TESTING MACHINE MEMORY WITH TTK91 AND TTK15 FORMATS"

for test in $(cat test.cfg | cut -f1 -d' '); do

	
