NOTICE: This project is still WIP. Specifically, integration is still rather shaky (everything compiles and links for desktop, but mobile still needs work. Tried FreeBSD recently, no dice.) Therefore, starting a project now _might_ break code later and require refactoring on the end-user's part.

I'm developing a rather minimalistic cross-platform game/simulation engine. This toolkit is made specifically to reduce wasted time *cough cough* aside from the time it took to design and build this software *cough cough*.Much of it consists of integrating external libraries, most of which are in C++11. It currently runs on Linux and Android. We can expect Windows, Mac, and iOS support within the month. As a design goal, the legalese is designed to be extremely liberal (Apache 2.0.) I spend a good portion of my waking life (and a few of my dreams) in it, so expect active work.


Specific features:
```
A highly tunable gradient mapping shader by default. Comes with three XY-tileable ntextures blended into one.
Triplanar shader for non-UV mapped objects. Texture reads are kept to a minimum (3) by using the gradient mapping technique.
No significant obstacles between the programmer, the lower-level libraries and the data.
```

Currently brings in (among others):
```
Bgfx - Amazing cross-platform graphics solution!
glfw - For desktop
Bullet - Physics
Freetype - Fonts.
polyvox - 3D voxel worlds with mesh extraction
Lemon Graph Library 
Asio for solid networking
Assimp (and a working mesh loader, too! =P)
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

Drawables are minimal, and neatly managed. Most of the game features are to also be run headless. This project is focused on gettings things to work without the obstruction caused by deep inheritance hierarchies. This encourages rapid extendability. Compile times are manageable thanks to CMake caching goodness.

```

As for its future, I plan on continuing work on it quite vigorously. It should be able to port apps to the browser by fall and should trivially be made to work headless. The current focus is on making games.

Things to do in the immediate future:
```
Continue refining the simulation world.
Sound
Serialization
```

Medium future
```
Ports to Win, MacOS, iOS, Emscripten, NaCL
Test framework
Destructible worlds
```

Long-term
```
Dataviz
Cutting edge/esoteric programming idioms
```
