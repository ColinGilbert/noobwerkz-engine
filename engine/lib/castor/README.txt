Castor v1.1, Aug 15th 2010 : 

Copyright (C) 2008, 2010 Roshan Naik

License
============================
Distributed under The MIT License (http://www.opensource.org/licenses/mit-license.php)
See LICENSE.txt for details.

Visit http://www.mpprogramming.com for the most recent version of the library and
documentation.


About Castor
============================
Castor is a pure C++ library that enables the Logic paradigm in C++.
Since it is a pure header library there is no need to build Castor separately 
into a shared or static library. Also Castor does not depend on any library other 
than the standard C++ library.

Extract files from the archive and add the "castor1.1" directory to your compiler's include 
path. To make the library's facilities available to your application #include <castor.h>.
All facilites provided by Castor are part of the "castor" namespace.

The following is a trivial hello world program that uses Castor.

#include<castor.h>
using namespace castor;

int main() {
  relation r = write("hello world\n");
  r();
  return 0;
}


Documentation
========================
Please refer to the castor1.1\docs directory for the following:

  - "Whats new.txt" : Full list of the features introduced in Castor 1.1

  - Tutorial : CastorTutorial.pdf

  - Reference Manual: CastorReference.pdf

  - Design and implementation of Castor: CastorDesign.pdf



Tested with compilers:
========================
Microsoft Visual C++ 2005, 2008 and 2010
GCC 4.3.3, 4.4.1, 4.4.3



Compiling Castor test suite:
============================

First cd to castor1.1/test directory.


Microsoft C++ (on Windows)
--------------------------------------
From command line:
  To compile:   cl /I..\ /EHsc  *.cpp /Fetest_castor
  To Run    :   test_castor.exe



GCC  (on Unix)
--------------------------------------
  To compile:   g++ -I../ *.cpp -o test_castor 
  To Run    :   ./test_castor

