#installation#

##with git repository and makefile##
There is installation section in the project makefile so installation is very simple: When project is built (make is run) just type "make install" in the project root
and the binaries are installed to /usr/local/bin
Respectively the ttk-15 can be removed with "make remove".

NOTE! Installation to /usr/local/bin allmost allways requires superuser privileges so install it in root or use sudo.

There is also .deb and .rpm x86_64 packages for users who only needs a binaries of the project. (or does not want to install ncurses-dev)

##with red hat based systems##
In rpm based distros one can install this project from target/VERSION/x86_64/ttk-VERSION.x86_64.rpm with yum:

rpm -i package.rpm

##with debian based systems##
There is also .deb packages for x86_64. installation with dpkg is simple:

dpkg -i package.deb

##manual installation##
In the target/VERSION/ directory one can find all binaries that are precompiled. They can be used in that directory or one can install them wherever he/she wants.

In the version 0.3 and later there is at least following linux binaries:

 * armv7
 * armv6
 * i386
 * x86_64 (including .deb and .rpm)