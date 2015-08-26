#!/bin/bash

mv ~/rpmbuild/RPMS/$(uname -m)/ttk-$1-1.$(uname -m).rpm target/v$1/$(uname -m)/linux/
