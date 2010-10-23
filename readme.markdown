Multiary Wavelet Tree and RRR in C++
====================================

v0.3

Author: Alex Bowe

Email:  bowe.alexander@gmail.com

Changes
-------  
 * Multi-01RRR has intermediate counts to improve speed
 * Compressed
 * Variant data structures

Plans
-----   
 * Reduce WT construction memory (stop using filtered strings, maybe use bitmaps
   instead)
 * Pre-generate CountCube

Obtaining and Contributing
--------------------------  
To clone this repository:

	$ git clone http://github.com/alexbowe/multiary-wt.git
	
This will create a directory `multiary-wt` in your working directory. Note that this won't allow you to submit changes to the master repository.

If you want to contribute, please fork the project and submit a pull request. If I'm working with you at RMIT, let me know your GitHub account name and I will add you as a contributor.


Building With SCons Installed
-----------------------------  
To build, in your shell, type:
	
	$ scons
	
To build in release mode:

	$ scons DEBUG=0

Default debug level is 1.

To clean up:

	$ scons -c


Dependencies
------------  
These dependencies ARE NOT provided:

 * [Python](http://www.python.org) for build system only
 * [LaTeX](http://www.latex-project.org/) and [BibTeX](http://www.bibtex.org/) for the thesis itself
 * [SCons](http://www.scons.org) is the build system
 * [CppUnitLite](http://www.objectmentor.com/resources/downloads.html) for unit testing
 * [Boost](www.boost.org) for various C++ libraries


License
-------  
 * All of my work is licensed under the [Creative Commons Attribution-Share Alike 3.0 license](http://creativecommons.org/licenses/by-sa/3.0/)


Shout Outs
----------  
Special thanks to Simon Puglisi, Juha Kärkkäinen, and Nikolas Bowe for their guidance.
