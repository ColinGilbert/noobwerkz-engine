NOTICE: This project is still early-phase. Specifically, integration is still rather shaky (everything compiles and links for desktop, but mobile still needs work. Tried FreeBSD recently, no dice.) Therefore, starting a project now _might_ break code later and require refactoring on the end-user's part.

I'm developing a cross-platform game/simulation/dataviz platform, with a focus on pretty front-ends while still easily running headless. This toolkit is made specifically to reduce wasted time and to encourage code reuse in (what I hope) is an elegant way. Much of it consists of integrating external libraries, most of which are in C++11. It currently runs on Linux and Android. We can expect Windows, Mac, and iOS support within the month. As a design goal, the legalese is designed to be extremely liberal (Apache 2.0.)  I expect to spend a good portion of my waking life (and some of my dreams) in it, so expect active work.

This toolkit basically picks up libraries and brings them into a programmer's playground. One big feature is bgfx cross-platform rendering. Most of the work I do on this project consists of searching for good, liberally-licensed libraries that fit within scope add integrating them.

Specific features:
```
A highly tunable gradient mapping shader by default. Comes with three XY-tileable ntextures blended into one.
Triplanar shader for non-UV mapped objects. Texture reads are kept to a minimum (3) by using only the gradient mapping technique.
No obstacles between the programmer, the lower-level libraries and the data.
```

Currently brings in (among others):
```
Bgfx - Amazing cross-platform graphics solution!
glfw - For desktop
assimp - For loading and conditioning 3d assets
Freetype - Excellent font-handling.
Eigen - Darn good math library
Noise++ - Fastest 3D noise library in the world. Intend on upgrading soon.
polyvox - For 3D voxel worlds with mesh extraction
rbdl - For efficient simulation of linked chains of bodies
Lemon Graph Library - One of the best graph library around
Voro++ - Fast and well-documented Voronoi-regions utility
Asio for solid networking
Assimp (and a working mesh loader, too! =P)
csgjs-cpp for a neat little CSG library 
libpd and RtAudio for sound
cppformat - Fastest formatting since printf() but way better!
ChaiScript - Scripting that resembles C++, but with functional features too.
```

To clone:
```
git clone --recursive https://github.com/ColinGilbert/noobwerkz-engine.git
```

Program structure:
```
The current setup uses a platform-specific starter program to launch the main application.

There are many libraries - mostly math and scientific libraries - in the engine/lib directory.

For the desktop, there is also graphics and UI code, much of it cheerfully delegated to other libraries  - such as the aforementioned bgfxi and Assimp.

Drawables are minimal, and neatly managed. Most of the game features are to also be run headless. This project is focused on gettings things to work without the obstruction caused by deep inheritance hierarchies. This encourages rapid extendability. Compile times are manageable thanks to CMake caching goodness.

I contend that the little network logger powered by Asio might be the most important aspect of this toolkit. :P
```

As for its future, I plan on continuing work on it quite vigorously. It should be able to port apps to the browser by mid-summer and can trivially be made to work headless. The current focus on making games, but is expected to go towards ERP, automation, statistics, network management, and dataviz as my own needs change.

Things to do in the immediate future:
```
Ports to Win, MacOS, iOS, Emscripten, NaCL
Skeletal animation
```

Medium future
```
Add more constraint solvers - Gecode & Ceres
Functional C++ programming
Test framework
```

Long-term
```
Bro-IDS integration
Database backends
Application builds itself
Scheme implementation built on top
