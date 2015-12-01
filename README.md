Hi,

I am developing a solid, small-API, easy-to-use cross-platform game engine. This toolkit is made specifically to reduce wasted time (*cough cough* aside from the time it took to design and build this software *cough cough*.) Much of it consists of integrating external libraries, most of which are in C++11. It currently runs on Linux and Android. We can expect Windows, Mac, and iOS support quite soon. As a design goal, the legalese is designed to be extremely liberal (Apache 2.0.)

Still WIP. Not yet recommended for use. Please feel free to bookmark this repo and come by later because this thing is living with me and awesome things will soon come out of it. :)

Documentation is rather sparse at the moment, and consists of code comments, this page, and issues on the tracker. The issues are well-written, thoughtfully labeled, and organized into the right milestones. Link:

https://github.com/ColinGilbert/noobwerkz-engine/milestones

These can provide a sense of direction as to the direction of the project; I encourage you to browse in there.

High-quality tutorials and real unit tests will come when the implementation is set in stone.

Current version: 0.0.1.1, AKA: Fresh "beginnings."

Specific features:
```
A highly tunable gradient mapping shader by default. Comes with three XY-tileable textures blended into one.
Triplanar shader for non-UV mapped objects. Texture reads are kept to a minimum (3) by using the gradient mapping technique.
Provides clean interfaces over to those libraries and tools on top, with varying degrees of safety. These choices are left to the programmer.
```

Currently brings in (among others):
```
Bgfx
glfw
Bullet Physics
Freetype
polyvox
Lemon Graph Library 
Assimp (and a working mesh loader, too! =P)
cppformat
Cereal
AngelScript
```

To clone:
```
git clone --recursive https://github.com/ColinGilbert/noobwerkz-engine.git
```

Structure:
```
The current setup uses a platform-specific starter program to launch the main application.

There are many libraries - mostly utility libraries - in the engine/lib directory. Not all of them are being used. This needs a cleanup, but at the moment this is still my personal dev repo. Will modularize out someday.

Most engine code written in engine/common directory.

Short scripts with tiny names are used to do routine work (compiles, archivals, cleanups.) Will eventually bring it into C++ and AngelScript.
```

Platforms:
```
Current setup uses linux as main controller.
Ports soon.
```

Little *nix scripts (currently runs on bash but I don't believe it uses any bashisms):
```
sandbox/b for build. Attempts to build source
sandbox/bb for big build. Wipes the binaries and builds from scratch
sandbox/s for shaders. Builds the full set of shaders from sandbox/shader_src and places them into the sandbox/shaders directory
sandbox/g for git. Adds changes to git and commits them
```

Things to do in the immediate future:
```
Continue refining the simulation world
AI
Sound
Serialization
Scripting
```

Medium future:
```
Ports to Win, MacOS, iOS, Emscripten, NaCL
Test framework
Destructible worlds
Basic AI
```

Long-term:
```
I intend on splitting out the platform-specific code from this project into it's own middleware platform, and running the game engine on top of it.
The focus is for people wanting to set up a proper coding lab, with a specific minimum hardware investment being the established norm. 
Shall also come with blueprints for suggested network layouts.
```

Short-term uses:
```
Game engine and content creation.
```

Long-term uses:
```
Middleware between disparate OS'es and their ecosystems: Will include support for interfacing with *nix, Android, Windows, Apple, and Steam ecosystems (not just the base OSes)
Automated testing/simulation
Controlling state across networks
Data Analysis/DataViz
Accounting (both network events and finances)
Secure smart homes
Other business needs
```
