.. -*- mode: rst -*-
.. This text is in reStucturedText format, so it may look a bit odd.
.. See http://docutils.sourceforge.net/rst.html for details.

=============================
UNIX/Cygwin/MinGW Compilation
=============================

.. contents::
  :local:

Archive Formats
---------------

GraphicsMagick is distributed in a number of different archive formats.
The source code must be extracted prior to compilation as follows:

7z

    7-Zip archive format. The Z-Zip format may be extracted under Unix
    using '7za' from the P7ZIP package (http://p7zip.sourceforge.net/).
    Extract similar to::

      7za x GraphicsMagick-1.3.7z

.tar.bz2

    BZip2 compressed tar archive format. Requires that both the bzip2
    (http://www.bzip.org/) and tar programs to be available. Extract
    similar to::

      bzip2 -d GraphicsMagick-1.3.tar.bz | tar -xvf -

.tar.gz

    Gzip compressed tar archive format. Requires that both the gzip
    (http://www.gzip.org/) and tar programs to be available. Extract
    similar to::

      gzip -d GraphicsMagick-1.3.tar.gz | tar -xvf -

.tar.lz

    Lzip compressed tar archive format.  Requires that both the lzip
    (http://lzip.nongnu.org/lzip.html) and tar programs to be
    available. Extract similar to::

      lzip -d -c GraphicsMagick-1.3.tar.gz | tar -xvf -

.tar.xz

    LZMA compressed tar archive format. Requires that LZMA utils
    (http://tukaani.org/lzma/) and tar programs to be available. Extract
    similar to::

      xz -d GraphicsMagick-1.3.tar.xz | tar -xvf -

zip

    PK-ZIP archive format. Requires that the unzip program from Info-Zip
    (http://www.info-zip.org/UnZip.html) be available. Extract similar to::

      unzip GraphicsMagick-1.3.zip

The GraphicsMagick source code is extracted into a subdirectory
similar to 'GraphicsMagick-1.3'. After the source code extracted,
change to the new directory (using the actual directory name) using
a command similar to::

  cd GraphicsMagick-1.3


Build Configuration
-------------------

Use 'configure' to automatically configure, build, and install
GraphicsMagick. The configure script may be executed from the
GraphicsMagick source directory (e.g ./configure) or from a separate
build directory by specifying the full path to configure (e.g.
/src/GraphicsMagick-1.3/configure). The advantage of using a separate
build directory is that multiple GraphicsMagick builds may share the
same GraphicsMagick source directory while allowing each build to use a
unique set of options.  Using a separate directory also makes it easier
to keep track of any files you may have edited.

If you are willing to accept configure's default options (static
build, 8 bits/sample), and build from within the source directory,
type::

    ./configure

and watch the configure script output to verify that it finds everything
that you think it should. If it does not, then adjust your environment
so that it does.

By default, 'make install' will install the package's files
in '/usr/local/bin', '/usr/local/man', etc. You can specify an
installation prefix other than '/usr/local' by giving 'configure'
the option '--prefix=PATH'.  This is valuable in case you don't have
privileges to install under the default paths or if you want to install
in the system directories instead.

If you are not happy with configure's choice of compiler, compilation
flags, or libraries, you can give 'configure' initial values for
variables by specifying them on the configure command line, e.g.::

    ./configure CC=c99 CFLAGS=-O2 LIBS=-lposix

Options which should be common to packages installed under the same
directory heirarchy may be supplied via a 'config.site' file located
under the installation prefix via the path ${prefix}/share/config.site
where ${prefix} is the installation prefix. This file is used for all
packages installed under that prefix. As an alternative, the CONFIG_SITE
environment variable may be used to specify the path of a site
configuration file to load. This is an example config.site file::

  # Configuration values for all packages installed under this prefix
  CC=gcc
  CXX=c++
  CPPFLAGS='-I/usr/local/include'
  LDFLAGS='-L/usr/local/lib -R/usr/local/lib'

When the 'config.site' file is being used to supply configuration
options, configure will issue a message similar to::

  configure: loading site script /usr/local/share/config.site

The configure variables you should be aware of are:

CC

    Name of C compiler (e.g. 'cc -Xa') to use

CXX

    Name of C++ compiler to use (e.g. 'CC')

CFLAGS

    Compiler flags (e.g. '-g -O2') to compile C code

CXXFLAGS

    Compiler flags (e.g. '-g -O2') to compile C++ code

CPPFLAGS

    Include paths (-I/somedir) to look for header files

LDFLAGS

    Library paths (-L/somedir) to look for libraries Systems that
    support the notion of a library run-path may require an additional
    argument in order to find shared libraries at run time. For
    example, the Solaris linker requires an argument of the form
    '-R/somedir', some Linux systems will work with '-rpath /somedir',
    while some other Linux systems who's gcc does not pass -rpath to
    the linker require an argument of the form '-Wl,-rpath,/somedir'.

LIBS

    Extra libraries (-lsomelib) required to link

Any variable (e.g. CPPFLAGS or LDFLAGS) which requires a directory
path must specify an absolute path rather than a relative path.

The build now supports a Linux-style "silent" build (default
disabled).  To enable this, add the configure option
--enable-silent-rules or invoke make like 'make V=0'.  If the build
has been configured for silent mode and it is necessary to see a
verbose build, then invoke make like 'make V=1'.

Configure can usually find the X include and library files
automatically, but if it doesn't, you can use the 'configure' options
'--x-includes=DIR' and '--x-libraries=DIR' to specify their locations.

The configure script provides a number of GraphicsMagick specific
options.  When disabling an option --disable-something is equivalent
to specifying --enable-something=no and --without-something is
equivalent to --with-something=no.  The configure options are as
follows (execute 'configure --help' to see all options).


Optional Features
~~~~~~~~~~~~~~~~~

--enable-prof

    enable 'prof' profiling support (default disabled)

--enable-gprof

    enable 'gprof' profiling support (default disabled)

--enable-gcov

    enable 'gcov' profiling support (default disabled)

--disable-installed

    disable building an installed GraphicsMagick (default enabled)

--enable-broken-coders

    enable broken/dangerous file formats support

--disable-largefile

    disable support for large (64 bit) file offsets

--disable-openmp

    disable use of OpenMP (automatic multi-threaded loops) at all

--enable-openmp-slow

    enable OpenMP for algorithms which sometimes run slower

--enable-symbol-prefix

    enable prefixing library symbols with "Gm"

--enable-magick-compat

    install ImageMagick utility shortcuts (default disabled)

--enable-maintainer-mode

    enable additional Makefile rules which update generated files
    included in the distribution. Requires GNU make as well as a
    number of utilities and tools.

--enable-quantum-library-names

    shared library name includes quantum depth to allow shared
    libraries with different quantum depths to co-exist in same
    directory (only one can be used for development)


Optional Packages/Options
~~~~~~~~~~~~~~~~~~~~~~~~~

--with-quantum-depth

    number of bits in a pixel quantum (default 8).  Also see
    '--enable-quantum-library-names.'

--with-modules

    enable building dynamically loadable modules

--without-threads

    disable POSIX threads API support

--with-frozenpaths

    enable frozen delegate paths

--without-magick-plus-plus

    disable build/install of Magick++

--with-perl

    enable build/install of PerlMagick

--with-perl=PERL

    use specified Perl binary to configure PerlMagick

--with-perl-options=OPTIONS

    options to pass on command-line when generating PerlMagick's Makefile from Makefile.PL

--without-bzlib

    disable BZLIB support

--without-dps

    disable Display Postscript support

--with-fpx

    enable FlashPIX support

--with-gslib

    enable Ghostscript library support (not recommended)

--without-jbig

    disable JBIG support

--without-jp2

    disable JPEG v2 support

--without-jpeg

    disable JPEG support

--without-jp2

    disable JPEG v2 support

--without-lcms2

    disable lcms (v2.X) support

--without-lzma

    disable LZMA support

--without-png

    disable PNG support

--without-tiff

    disable TIFF support

--without-trio

    disable TRIO library support

--without-ttf

    disable TrueType support

--with-umem

    enable libumem memory allocation library support

--without-wmf

    disable WMF support

--with-fontpath

    prepend to default font search path

--with-gs-font-dir

    directory containing Ghostscript fonts

--with-windows-font-dir

    directory containing MS-Windows fonts

--without-xml

    disable XML support

--without-zlib

    disable ZLIB support

--with-x

    use the X Window System

--with-share-path=DIR

    Alternate path to share directory (default share/GraphicsMagick)

--with-libstdc=DIR

    use libstdc++ in DIR (for GNU C++)

GraphicsMagick options represent either features to be enabled, disabled,
or packages to be included in the build.  When a feature is enabled (via
--enable-something), it enables code already present in GraphicsMagick.
When a package is enabled (via --with-something), the configure script
will search for it, and if is is properly installed and ready to use
(headers and built libraries are found by compiler) it will be included
in the build.  The configure script is delivered with all features
disabled and all packages enabled. In general, the only reason to
disable a package is if a package exists but it is unsuitable for
the build (perhaps an old version or not compiled with the right
compilation flags).

Several configure options require special note:

--enable-shared

  The shared libraries are built and support for loading coder and
  process modules is enabled. Shared libraries are preferred because
  they allow programs to share common code, making the individual
  programs much smaller. In addition shared libraries are required in
  order for PerlMagick to be dynamically loaded by an installed PERL
  (otherwise an additional PERL (PerlMagick) must be installed. This
  option is not the default because all libraries used by
  GraphicsMagick must also be dynamic libraries if GraphicsMagick
  itself is to be dynamically loaded (such as for PerlMagick).

  GraphicsMagick built with delegates (see MAGICK PLUG-INS below)
  can pose additional challenges. If GraphicsMagick is built using
  static libraries (the default without --enable-shared) then
  delegate libraries may be built as either static libraries or
  shared libraries. However, if GraphicsMagick is built using shared
  libraries, then all delegate libraries must also be built as
  shared libraries.  Static libraries usually have the extension .a,
  while shared libraries typically have extensions like .so, .sa,
  or .dll. Code in shared libraries normally must compiled using
  a special compiler option to produce Position Independent Code
  (PIC). The only time this is not necessary is if the platform
  compiles code as PIC by default.

  PIC compilation flags differ from vendor to vendor (gcc's is
  -fPIC). However, you must compile all shared library source with
  the same flag (for gcc use -fPIC rather than -fpic). While static
  libraries are normally created using an archive tool like 'ar',
  shared libraries are built using special linker or compiler options
  (e.g. -shared for gcc).

  Building shared libraries often requires subtantial hand-editing
  of Makefiles and is only recommended for those who know what they
  are doing.

  If --enable-shared is not specified, a new PERL interpreter
  (PerlMagick) is built which is statically linked against the
  PerlMagick extension. This new interpreter is installed into the
  same directory as the GraphicsMagick utilities. If --enable-shared
  is specified, the PerlMagick extension is built as a dynamically
  loadable object which is loaded into your current PERL interpreter
  at run-time. Use of dynamically-loaded extensions is preferable over
  statically linked extensions so --enable-shared should be specified
  if possible (note that all libraries used with GraphicsMagick must
  be shared libraries!).

--disable-static

  static archive libraries (with extension .a) are not built. If you
  are building shared libraries, there is little value to building
  static libraries. Reasons to build static libraries include: 1) they
  can be easier to debug; 2) the clients do not have external
  dependencies (i.e. libMagick.so); 3) building PIC versions of the
  delegate libraries may take additional expertise and effort; 4) you
  are unable to build shared libraries.

--disable-installed

  By default the GraphicsMagick build is configured to formally install
  into a directory tree. This is the most secure and reliable way to
  install GraphicsMagick. Specifying --disable-installed configures
  GraphicsMagick so that it doesn't use hard-coded paths and locates
  support files by computing an offset path from the executable (or
  from the location specified by the MAGICK_HOME environment variable.
  The uninstalled configuration is ideal for binary distributions which
  are expected to extract and run in any location.

--enable-broken-coders

  The implementation of file format support for some formats is
  incomplete or imperfectly implemented such that file corruption or a
  security exploit might occur.  These formats are not included in the
  build by default but may be enabled using
  ``--enable-broken-coders``.  The existing implementation may still
  have value in controlled circumstances so it remains but needs to be
  enabled.  One of the formats currently controlled by this is Adobe
  Photoshop bitmap format (PSD).

--with-modules

  Image coders and process modules are built as loadable modules which
  are installed under the directory
  [prefix]/lib/GraphicsMagick-X.X.X/modules-QN (where 'N' equals 8, 16,
  or 32 depending on the quantum depth) in the subdirectories 'coders'
  and 'filters' respectively. The modules build option is only
  available in conjunction with --enable-shared. If --enable-shared is
  not also specified, then support for building modules is disabled.
  Note that if --enable-shared is specified, the module loader is
  active (allowing extending an installed GraphicsMagick by simply
  copying a module into place) but GraphicsMagick itself is not built
  using modules.

--enable-symbol-prefix

  The GraphicsMagick libraries may contain symbols which conflict with
  other libraries. Specifify this option to prefix "Gm" to all library
  symbols, and use the C pre-processor to allow dependent code to still
  compile as before.

--enable-magick-compat

  Normally GraphicsMagick installs only the 'gm' utility from which all
  commands may be accessed. Existing packages may be designed to invoke
  ImageMagick utilities (e.g. "convert"). Specify this option to
  install ImageMagick utility compatibility links to allow
  GraphicsMagick to substitute directly for ImageMagick. Take care when
  selecting this option since if there is an existing ImageMagick
  installation installed in the same directory, its utilities will be
  replaced when GraphicsMagick is installed.

--with-quantum-depth

  This option allows the user to specify the number of bits to use per
  pixel quantum (the size of the red, green, blue, and alpha pixel
  components. When an image file with less depth is read, smaller
  values are scaled up to this size for processing, and are scaled
  down from this size when a file with lower depth is written.  For
  example, "--with-quantum-depth=8" builds GraphicsMagick using 8-bit
  quantums. Most computer display adaptors use 8-bit
  quantums. Currently supported arguments are 8, 16, or 32.  The
  default is 8. This option is the most important option in
  determining the overall run-time performance of GraphicsMagick.

  The number of bits in a quantum determines how many values it may
  contain. Each quantum level supports 256 times as many values as
  the previous level. The following table shows the range available
  for various quantum sizes.

      ============  =====================  =================
      QuantumDepth  Valid Range (Decimal)  Valid Range (Hex)
      ============  =====================  =================
            8                0-255               00-FF
           16               0-65535            0000-FFFF
           32            0-4294967295      00000000-FFFFFFFF
      ============  =====================  =================

  Larger pixel quantums cause GraphicsMagick to run more slowly and to
  require more memory. For example, using sixteen-bit pixel quantums
  causes GraphicsMagick to run 15% to 50% slower (and take twice as
  much memory) than when it is built to support eight-bit pixel
  quantums.  Regardless, the GraphicsMagick authors prefer to use
  sixteen-bit pixel quantums since they support all common image
  formats and assure that there is no loss of color precision.

  The amount of virtual memory consumed by an image can be computed
  by the equation (QuantumDepth*Rows*Columns*5)/8. This is an
  important consideration when resources are limited, particularly
  since processing an image may require several images to be in
  memory at one time. The following table shows memory consumption
  values for a 1024x768 image:

      ============  ==============
      QuantumDepth  Virtual Memory
      ============  ==============
          8              3MB
         16              8MB
         32             15MB
      ============  ==============

  GraphicsMagick performs all image processing computations using
  floating point or non-lossy integer arithmetic, so results are very
  accurate.  Increasing the quantum storage size decreases the amount
  of quantization noise (usually not visible at 8 bits) and helps
  prevent countouring and posterization in the image.

  Consider also using the --enable-quantum-library-names configure
  option so that installed shared libraries include the quantum depth
  as part of their names so that shared libraries using different
  quantum depth options may co-exist in the same directory.

--without-magick-plus-plus

  Disable building Magick++, the C++ application programming interface
  to GraphicsMagick. A suitable C++ compiler is required in order to
  build Magick++. Specify the CXX configure variable to select the C++
  compiler to use (default "g++"), and CXXFLAGS to select the desired
  compiler opimization and debug flags (default "-g -O2"). Antique C++
  compilers will normally be rejected by configure tests so specifying
  this option should only be necessary if Magick++ fails to compile.

--with-frozenpaths

  Normally external program names are substituted into the
  delegates.mgk file without full paths. Specify this option to enable
  saving full paths to programs using locations determined by
  configure. This is useful for environments where programs are stored
  under multiple paths, and users may use different PATH settings than
  the person who builds GraphicsMagick.

--without-threads

  By default, the GraphicsMagick library is compiled to be fully
  thread safe by using thread APIs to implement required locking.
  This is intended to allow the GraphicsMagick library to be used by
  multi-threaded programs using native POSIX threads. If the locking
  or dependence on thread APIs is undesireable, then specify
  --without-threads.  Testing shows that the overhead from thread
  safety is virtually unmeasurable so usually there is no reason to
  disable multi-thread support.  While previous versions disabled
  OpenMP support when this option was supplied, that is no longer the
  case.

--disable-largefile

  By default, GraphicsMagick is compiled with support for large (> 2GB
  on a 32-bit CPU) files if the operating system supports large files.
  Applications which use the GraphicsMagick library might then also
  need to be compiled to support for large files (operating system
  dependent).  Normally support for large files is a good thing.  Only
  disable this option if there is a need to do so.

--disable-openmp

  By default, GraphicsMagick is compiled with support for OpenMP
  (http://www.openmp.org/) if the compilation environment supports it.
  OpenMP automatically parallizes loops across concurrent threads
  based on instructions in pragmas. OpenMP was introduced in GCC
  4.2. OpenMP is a well-established standard and was implemented in
  some other compilers in the late '90s, long before its appearance in
  GCC. OpenMP adds additional build and linkage requirements.

  By default, GraphicsMagick enables as many threads as there are CPU
  cores (or CPU threads).  According to the OpenMP standard, the
  OMP_NUM_THREADS environment variable specifies how many threads
  should be used and GraphicsMagick also honors this request. In order
  to obtain the best single-user performance, set OMP_NUM_THREADS
  equal to the number of available CPU cores.  On a server with many
  cores and many programs running at once, there may be benefit to
  setting OMP_NUM_THREADS to a much smaller value than the number of
  cores, and sometimes values as low as two (or even one, to disable
  threading) will offer the best overall system performance.  Tuning a
  large system with OpenMP programs running in parallel (competing for
  resources) is a complex topic and some research and experimentation
  may be required in order to find the best parameters.

--enable-openmp-slow

  On some systems, memory-bound algorithms run slower (rather than
  faster) as threads are added via OpenMP.  This may be due to CPU
  cache and memory architecture implementation, or OS thread API
  implementation.  Since it is not known how a system will behave
  without testing and pre-built binaries need to work well on all
  systems, these algorithms are now disabled for OpenMP by default.
  If you are using a well-threaded OS on a CPU with a good
  high-performance memory architecture, you might consider enabling
  this option based on experimentation.

--with-perl

  Use this option to include PerlMagick in the GraphicsMagick build
  and test suite. While PerlMagick is always configured by default
  (PerlMagick/Makefile.PL is generated by the configure script),
  PerlMagick is no longer installed by GraphicsMagick's ''make
  install''.  The procedure to configure, build, install, and check
  PerlMagick is described in PerlMagick/README.txt.  When using a
  shared library build of GraphicsMagick, it is necessary to formally
  install GraphicsMagick prior to building PerlMagick in order to
  achieve a working PerlMagick since otherwise the wrong
  GraphicsMagick libraries may be used.

  If the argument ''--with-perl=/path/to/perl'' is supplied, then
  /path/to/perl will be taken as the PERL interpreter to use. This is
  important in case the 'perl' executable in your PATH is not PERL5,
  or is not the PERL you want to use.  Experience suggests that static
  PerlMagick builds may not be fully successful for Perl versions
  newer than 5.8.8.

--with-perl-options

  The PerlMagick module is normally installed using the Perl
  interpreter's installation PREFIX, rather than GraphicsMagick's. If
  GraphicsMagick's installation prefix is not the same as PERL's
  PREFIX, then you may find that PerlMagick's 'make install' step tries
  to install into a directory tree that you don't have write
  permissions to. This is common when PERL is delivered with the
  operating system or on Internet Service Provider (ISP) web servers.
  If you want PerlMagick to install elsewhere, then provide a PREFIX
  option to PERL's configuration step via
  "--with-perl-options=PREFIX=/some/place". Other options accepted by
  MakeMaker are 'LIB', 'LIBPERL_A', 'LINKTYPE', and 'OPTIMIZE'. See the
  ExtUtils::MakeMaker(3) manual page for more information on
  configuring PERL extensions.

--without-x

  By default, GraphicsMagick will use X11 libraries if they are
  available. When --without-x is specified, use of X11 is disabled. The
  display, animate, and import sub-commands are not included. The
  remaining sub-commands have reduced functionality such as no access
  to X11 fonts (consider using Postscript or TrueType fonts instead).

--with-gs-font-dir

  Specify the directory containing the Ghostscript Postscript Type 1
  font files (e.g. "n022003l.pfb") so that they can be rendered using
  the FreeType library. If the font files are installed using the
  default Ghostscript installation paths
  (${prefix}/share/ghostscript/fonts), they should be discovered
  automatically by configure and specifying this option is not
  necessary. Specify this option if the Ghostscript fonts fail to be
  located automatically, or the location needs to be overridden.

--with-windows-font-dir

  Specify the directory containing MS-Windows-compatible fonts. This is
  not necessary when GraphicsMagick is running under MS-Windows.

Building under Cygwin
---------------------

GraphicsMagick may be built under the Windows '95-XP Cygwin
Unix-emulation environment available for free from

    http://www.cygwin.com/

It is suggested that the X11R6 package be installed since this enables
GraphicsMagick's X11 support (animate, display, and import
sub-commands will work) and it includes the Freetype v2 DLL required
to support TrueType and Postscript Type 1 fonts. Make sure that
/usr/X11R6/bin is in your PATH prior to running configure.

If you are using Cygwin version 1.3.9 or later, you may specify the
configure option '--enable-shared' to build Cygwin DLLs. Specifying
'--enable-shared' is required if you want to build PerlMagick under
Cygwin because Cygwin does not provide the libperl.a static library
required to create a static PerlMagick.  Note that older Cygwin
compilers may not generate code which supports reliably catching C++
exceptions thrown by DLL code.  The Magick++ library requires that it
be possible to catch C++ exceptions thrown from DLLs.  The test suite
``make check`` includes several tests to verify that C++ exceptions
are working properly.

Building under MinGW & MSYS
---------------------------

GraphicsMagick may be built using the free MinGW ("Minimalistic GNU for
Windows") package, available from

    http://www.mingw.org/

or from

    http://mingw-w64.sourceforge.net/

which consist of GNU-based (GCC) compilation toolsets plus headers and
libraries required to build programs which are entirely based on
standard Microsoft Windows DLLs so that they may be used for
proprietary applications. MSYS provides a Unix-style console shell
window with sufficient functionality to run the GraphicsMagick
configure script and execute 'make', 'make check', and 'make install'.
GraphicsMagick may be executed from the MSYS shell, but since it is a
normal Windows application, it will work just as well from the Windows
command line.

Unlike the Cygwin build which creates programs based on a
Unix-emulation DLL, and which uses Unix-style paths to access Windows
files, the MinGW build creates native Windows console applications
similar to the Visual C++ build. Run-time performance is similar to the
Microsoft compilers.

The base MinGW (or MinGW-w64) package and the MSYS package should be
installed. Other MinGW packages are entirely optional. Once MSYS is
installed a MSYS icon (blue capital 'M') is added to the
desktop. Double clicking on this icon starts an instance of the MSYS
shell.

Start the MSYS console and follow the Unix configure and build
instructions. The configure and build for MinGW is the same as for
Unix. Any additional delegate libraries (e.g. libpng) will need to be
built under MinGW in order to be used. These libraries should be built
and installed prior to configuring GraphicsMagick. While some delegate
libraries are easy to configure and build under MinGW, others may be
quite a challenge.

Lucky for us, the most common delegate libraries are available
pre-built, as part of the GnuWin32 project, from

    http://gnuwin32.sourceforge.net/packages.html

The relevant packages are bzip2, freetype, jbigkit, libintl, jpeg,
libpng, libtiff, libwmf and zlib. However, note that for freetype
to be detected by configure, you must move the ``freetype`` directory
out of ``GnuWin32\include\freetype2`` and into ``GnuWin32\include``.

Note that older MinGW compilers may not generate code which supports
reliably catching C++ exceptions thrown by DLL code.  The Magick++
library requires that it be possible to catch C++ exceptions thrown
from DLLs.  The test suite (``make check``) includes several tests to
verify that C++ exceptions are working properly.  If the MinGW you are
using fails the C++ exception tests, then the solution is to either
find a MinGW with working C++ exceptions, configure a static build
with --disable-shared, or disable building Magick++ with
--without-magick-plus-plus.

Note that the default installation prefix is MSYS's notion of
``/usr/local`` which installs the package into a MSYS directory. To
install outside of the MSYS directory tree, you may specify an
installation prefix like ``/c/GraphicsMagick`` which causes the package
to be installed under the Windows directory ``C:\GraphicsMagick``. The
installation directory structure will look very much like the Unix
installation layout (e.g. ``C:\GraphicsMagick\bin``,
``C:\GraphicsMagick\lib``, ``C:\GraphicsMagick\share``, etc.). Paths
which may be embedded in libraries and configuration files are
transformed into Windows paths so they don't depend on MSYS.

Cross-compilation On Unix/Linux Host
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Given a modern and working MinGW32 or mingw-w64 installation, it is
easy to cross-compile GraphicsMagick from a Unix-type host to produce
Microsoft Windows executables.

This incantation produces a static WIN32 `gm.exe` executable on an
Ubuntu Linux host with the i686-w64 cross-compiler installed::

  ./configure '--host=i686-w64-mingw32' '--disable-shared'

and this incantation produces a static WIN64 `gm.exe` executable on an
Ubuntu Linux host with the x86_64-w64 cross-compiler installed::

  ./configure '--host=x86_64-w64-mingw32' '--disable-shared'

For a full-fledged GraphicsMagick program, normally one will want to
pre-install or cross-compile the optional libraries that
GraphicsMagick may depend on and install them where the cross-compiler
will find them, or add extra `CPPFLAGS` and `LDFLAGS` options so that
the compiler searches for header files and libraries in the correct
place.

Configuring for building with shared libraries (libGraphicsMagick,
libGraphicsMagickWand, and libGraphicsMagick++ DLLs) and modules
(coders as DLLs) is also supported by the cross-builds.  A cross-built
libtool libltdl needs to be built in advance in order to use the
`--with-modules` modules option.

After configuring the software for cross-compilation, the software is
built using `make` as usual and everything should be as with native
compilation except that `make check` is likely not available (testing
might be possible on build system via WINE, not currently
tested/supported by GraphicsMagick authors).

Use of the `DESTDIR` approach as described in the `Build & Install`_
section is recommended in order to install the build products into a
formal directory tree before preparing to copy onto the Windows target
system (e.g. by packaging via an installer).

Dealing with configuration failures
-----------------------------------

While configure is designed to ease installation of GraphicsMagick, it
often discovers problems that would otherwise be encountered later
when compiling GraphicsMagick. The configure script tests for headers
and libraries by executing the compiler (CC) with the specified
compilation flags (CFLAGS), pre-processor flags (CPPFLAGS), and linker
flags (LDFLAGS). Any errors are logged to the file 'config.log'. If
configure fails to discover a header or library please review this
log file to determine why, however, please be aware that *errors
in the config.log are normal* because configure works by trying
something and seeing if it fails. An error in config.log is only a
problem if the test should have passed on your system. After taking
corrective action, be sure to remove the 'config.cache' file before
running configure so that configure will re-inspect the environment
rather than using cached values.

Common causes of configure failures are:

1) A delegate header is not in the header include path (CPPFLAGS -I
   option).

2) A delegate library is not in the linker search/run path (LDFLAGS
   -L/-R option).

3) A delegate library is missing a function (old version?).OB

4) The compilation environment is faulty.

If all reasonable corrective actions have been tried and the problem
appears to be due to a flaw in the configure script, please send a
bug report to the configure script maintainer (currently
bfriesen@graphicsmagick.org). All bug reports should contain the
operating system type (as reported by 'uname -a') and the
compiler/compiler-version. A copy of the configure script output
and/or the config.log file may be valuable in order to find the
problem. If you send a config.log, please also send a script of the
configure output and a description of what you expected to see (and
why) so the failure you are observing can be identified and resolved.

Makefile Build Targets
----------------------

Once GraphicsMagick is configured, these standard build targets are
available from the generated Makefiles:

  'make'

     Build the package

  'make install'

     Install the package

  'make check'

     Run tests using the uninstalled software. On some systems, 'make
     install' must be done before the test suite will work but usually
     the software can be tested prior to installation.

  'make clean'

     Remove everything in the build directory created by 'make'

  'make distclean'

     Remove everything in the build directory created by 'configure'
     and 'make'. This is useful if you want to start over from scratch.

  'make uninstall'

     Remove all files from the system which are (or would be) installed
     by 'make install' using the current configuration. Note that this
     target does not work for PerlMagick since Perl no longer supports
     an 'uninstall' target.

Build & Install
---------------

Now that GraphicsMagick is configured, type ::

     make

to build the package and ::

     make install

to install it.

To install under a specified directory using the install directory
tree layout (e.g. as part of the process for packaging the built
software), specify DESTDIR like ::

  make DESTDIR=/my/dest/dir install

Verifying The Build
-------------------

To confirm your installation of the GraphicsMagick distribution was
successful, ensure that the installation directory is in your executable
search path and type ::

  gm display

The GraphicsMagick logo should be displayed on your X11 display.

Verify that the expected image formats are supported by executing ::

  gm convert -list formats

Verify that the expected fonts are available by executing ::

  gm convert -list fonts

Verify that delegates (external programs) are configured as expected
by executing ::

  gm convert -list delegates

Verify that color definitions may be loaded by executing ::

  gm convert -list colors

If GraphicsMagick is built to use loadable coder modules, then verify
that the modules load via ::

  gm convert -list modules

Verify that GraphicsMagick is properly identifying the resources of
your machine via ::

  gm convert -list resources

For a thorough test, you should run the GraphicsMagick test suite by
typing ::

  make check

Note that due to differences between the developer's environment and
your own, it is possible that some tests may be indicated as failed
even though the results are ok.  Such failures should be rare, and if
they do occur, they should be reported as a bug.  Differences between
the developer's environment environment and your own may include the
compiler, the CPU type, and the library versions used. The
GraphicsMagick developers use the current release of all dependent
libraries.


.. |copy|   unicode:: U+000A9 .. COPYRIGHT SIGN

Copyright |copy| GraphicsMagick Group 2002 - 2016
