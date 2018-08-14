# installation

## with git repository and makefile
There is installation section in the project makefile so installation is very simple: When project is built (make is run) just type "make install" in the project root
and the binaries are installed to /usr/local/bin
Respectively the ttk-15 can be removed with "make remove".

NOTE! Installation to /usr/local/bin allmost allways requires superuser privileges so install it in root or use sudo.

There is also .deb and .rpm x86_64 packages for users who only needs a binaries of the project. (or does not want to install ncurses-dev)

## with red hat based systems
In rpm based distros one can install this project from target/VERSION/x86_64/linux/ttk-VERSION.x86_64.rpm with yum:

rpm -i package.rpm

## with debian based systems
There is also .deb packages for x86_64. installation with dpkg is simple:

dpkg -i package.deb

## manual installation
In the target/VERSION/ directory one can find all binaries that are precompiled. They can be used in that directory or one can install them wherever he/she wants.

In the version 0.3 and later there is at least following linux binaries:

 * armv7
 * armv6
 * i386
 * x86_64 (including .deb and .rpm)

# running

Running ttk-91 binaries does not need any modification or use of compiler or linker. ttk-15 machine can run ttk-91 (titokones format) binaries out of the box.
Just run "ttk-15 my_ttk-91_binary" and it should work. See more details using ttk-15 in doc/machine folder.

Compiling and running ttk-91 software from source: To get binaries you need to compile your .k91 vit compiler "ttc program.k91" to get object module. After that
you need to link the .o15 object module to linker. run "linker program.o15" and it will procude a.out.b15. That is your binary. To run it just type "ttk-15 a.out.b15"

See more details about program flags from dev/machine dev/linker and dev/compiler directories.

NOTE! instructions above assumes you have installed the project to your system. If you want to use them without global installation, you can build the project,
go to target/ folder and do the same but "./ttk-15" instead of "ttk-15" and so on.
