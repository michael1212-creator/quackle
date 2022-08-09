Quackle   ![Icon](https://github.com/quackle/quackle/raw/master/IconSmall.png)
=======

[![AppVeyor build status](https://ci.appveyor.com/api/projects/status/ttcu5vruvcdljwel/branch/master?svg=true)](https://ci.appveyor.com/project/jfultz/quackle/branch/master)
[![CI builds](https://github.com/quackle/quackle/workflows/CI%20builds/badge.svg)](https://github.com/quackle/quackle/actions?query=workflow%3A%22CI+Builds%22+event%3Apush)

Crossword game artificial intelligence and analysis tool.

See LICENSE in this directory.

Building Quackle:
-----------------
Quackle runs automated GitHub CI builds on Qt 5.12 and 5.15, so it should work with any Qt version in that range.
See README.MacOS and README.Windows for platform-specific instructions.  Generally:

Install Qt by:

    sudo apt-get install qt5-qmake
    sudo apt-get install qtbase5-dev

Clone the repo or download the tarball and untar.  Use qmake to build quackle.pro and quackleio/quackleio.pro:

	qmake quackle.pro && make
	cd quackleio && qmake && make && cd ..

Finally, build the main binary, which will be inside the quacker directory:

	cd quacker && qmake && make

i.e.

    rm -f quacker/Quackle && qmake quackle.pro && make && cd quackleio && qmake && make && cd .. && cd quacker && qmake && make && cd ..
 
to clean:
    
    for dir in ./*; do (cd "$dir" && make clean); done

Also check out https://www.jetbrains.com/help/clion/qt-tutorial.html#qt-setup-in-clion for using Qt in CLion



The binary will build as 'Quackle'.  It might be found in the quacker directory or in the release subdirectory.


File organization:
------------------
* quackle/ - libquackle sources.  libquackle is the engine, and can be linked to any convenient interface.  It does not use Qt.
* quackle/quackleio/ - I/O library for Quackle.  Implements stuff for accessing dictionaries, serializing GCG files, etc.  Also, command-line option handling.  This does have some modest dependencies on Qt.
* quackle/quacker/ - code for full Quackle UI.  Written in Qt, and requires libquackleio and libquackle.
* quackle/makeminidawg/ - standalone console program for building Quackle dictionaries.
* quackle/makegaddag/ - standalone console program for building gaddag files.
* quackle/data/ - lexicons, strategy files, and alphabet resources for Quackle.
In this directory is libquackle. Run qmake and then run make in this directory. Then cd to quackle/quackleio/, run qmake, and then run make.


olaughlin@gmail.com
jasonkatzbrown@gmail.edu
jfultz@wolfram.com
matt.liberty@gmail.com
