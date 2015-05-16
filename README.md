NOTICE: This project is still early-phase. Specifically, integration plan is still rather shaky and over half of the targeted build systems aren't yet in place. Starting a project now _might_ break code later and require refactoring on the end-user's part. However, if you keep your code within your own directory in sandbox directory, you should be able to avoid a merge conflict. As a design goal, the legalese is designed to be extremely liberal (Apache 2.0) 

I'm developing a cross-platform game/simulation/dataviz platform, with a focus on pretty front-ends while still trivially being made able to run headless. This toolkit is made specifically to reduce wasted time and to encourage code reuse in (what I hope) is an elegant-enough way. It consists mostly of (more or less) integrating external libraries, most of which are in C++11. It currently runs on Linux and Android. We can expect Windows, Mac, and iOS support within the month. I expect to spend a good portion of my waking life (and some of my dreams) in it, so expect active work.

This toolkit basically picks up existing libraries with liberal licenses and brings them into one repo, with a few very basic tools on top. One big feature is bgfx cross-platform rendering. Most of the work I do on this project consists of searching for good, liberally-licensed libraries. I started developing in C, but then soon learned that if there's a good, fast library in existence to do anything useful, then the implementation is most often in C++. So, as not to waste time I started looking for the best libraries that open-source has to offer. These libraries then become a programmer's playground.

Specific features:
```
A highly tunable gradient mapping shader by default. Comes with three XY-tileable ntextures blended into one.
Triplanar shader for non-UV mapped coordinates.
No obstacles between the programmer, the core libraries and the data.
```

Currently brings in:
```
Bgfx
Eigen 
ShapeOp
Noise++
polyvox
Lemon Graph Library
rbdl
dlib
Voro++
Asio
Assimp (and a working mesh loader! =P)
FasTC (to be integrated more deeply)
csgjs-cpp
libpd
RtAudio
Cereal
```

To clone:
```
git clone --recursive https://github.com/ColinGilbert/noobwerkz-engine.git
```

Program structure:
```
The current setup uses a platform-specific starter program to launch the main application. For desktop, www.glfw.org is used as it works unobtrusively. However, I'm up for writing more entrypoints if doing so proves desirable. The rest is graphics management code, much of it delegated to other libraries such as Assimp and the previously-mentioned bgfx. Some drawables are defined and neatly managed, and an editor is being built. However, these components can easily be compiled out; the project is focused on gettings things to work without obstruction and rapid extendability. Compile times are manageable after the first compile. I contend that the little network logger powered by Asio (https://think-async.com/) might be the most important aspect of this toolkit. :P
```

As for its future, I plan on continuing work on it quite vigorously. It should be able to port apps to the browser by mid-summer and can trivially be made to work headless. Currently there is a strong focus on making games.




Things to do in the immediate future:
```
Setup more hardware environments (Win, MacOS, iOS, Emscriptem, )
Change the directory structure a little and rename a few files
Adopt CMake's ExternalProject_Add in order to integrate third-party more easily
GUI
Serialization (Cereal)
```

Stuff to choose and integrate:
```
Skeletal animation/IK code
Constraint solvers - likely Gecode
Functional programming
Test framework
```

Long-term
```
Bro integration
Database backends
Application builds itself
Scheme implementation

```
