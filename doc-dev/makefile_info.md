#root makefile#

The makefile in the project root works calling other makefiles. Machine, linker and compiler makefiles has
common sections: normal compilation, clean and clobber.

There is also install and remove sections that installs or uninstalls binaries to/from the system.

The test section calls to test/ directorys makefile. It runs all test in the test directory.

There is also sections for packaging binaries to .rpm and .deb. This section only works if one
have necessary tools installed and configured. build_deb should work in any environment where
debianutils and dpkg is installed. However the script does work properly in all architectures.
If there is problems with that ask apason for help. package_rpm should work if there is rpmbuild
directory configured in ones home directory and rpm packager is configured to put packages to
rpmbuild/RPMS folder. It also needs rpm and rpmdevtools to work.
