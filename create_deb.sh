#!/bin/bash

#determine build root and create subdirs
debian_root="ttk-$1-1"
mkdir -p $debian_root/DEBIAN

#insert version number and construct control file
cat control_head >> $debian_root/DEBIAN/control
echo Version: "$1"-1 >> $debian_root/DEBIAN/control
cat control_tail >> $debian_root/DEBIAN/control


#create subdirs
mkdir -p $debian_root/usr/local/bin

#copy binaries to package folder
cp target/ttk-15 $debian_root/usr/local/bin
cp target/ttc $debian_root/usr/local/bin
cp target/linker $debian_root/usr/local/bin

#and build
dpkg-deb --build ttk-$(echo "$1"-1)
