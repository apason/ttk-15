#!/bin/bash

if [ ! -e ~/rpmbuild ] ; then
    echo "rpmbuild not configured properly"
    exit -1
fi

rm -f ~/rpmbuild/SPECS/ttk.spec

cat spec.head >> ~/rpmbuild/SPECS/ttk.spec
echo "Version: $1" >> ~/rpmbuild/SPECS/ttk.spec
cat spec.tail >> ~/rpmbuild/SPECS/ttk.spec
echo "*  $(date | cut -f 1-3,6 -d " ") Arttu Kilpinen <arttu.kilpinen@helsinki.fi>" >> ~/rpmbuild/SPECS/ttk.spec
echo "- nothing" >> ~/rpmbuild/SPECS/ttk.spec


mkdir -p ~/rpmbuild/BUILDROOT/ttk-$1-1.$(uname -m)/usr/local/bin

cp ../target/linker ~/rpmbuild/BUILDROOT/ttk-$1-1.$(uname -m)/usr/local/bin
cp ../target/ttk-15 ~/rpmbuild/BUILDROOT/ttk-$1-1.$(uname -m)/usr/local/bin
cp ../target/ttc ~/rpmbuild/BUILDROOT/ttk-$1-1.$(uname -m)/usr/local/bin

rpmbuild -bb ~/rpmbuild/SPECS/ttk.spec

