#!/bin/bash

if [ ! -e "../target/v$1/$(uname -m)/linux/" ] ; then
    mkdir -p ../target/v$1/$(uname -m)/linux
fi

if [ ! -e "../target/$1/$(uname -m)/linux/ttk-$1-1.$(uname -m).rpm" ] ; then
    mv ~/rpmbuild/RPMS/$(uname -m)/ttk-$1-1.$(uname -m).rpm ../target/v$1/$(uname -m)/linux/
fi


