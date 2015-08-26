#!/bin/bash

if [ ! -e "../target/v$1/$(uname -m)/linux/" ] ; then
    mkdir -p ../target/v$1/$(uname -m)/linux
fi

if [ ! -e "../target/$1/$(uname -m)/linux/ttk-$1-1.deb" ] ; then
    mv ttk-$1-1.deb ../target/v$1/$(uname -m)/linux/
fi

rm -rdf ttk-$1-1
