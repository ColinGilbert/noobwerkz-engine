Hi,


I am developing a solid, fast, easy-to-use cross-platform game engine. This toolkit is made specifically to reduce wasted time (*cough cough* aside from the time it took to design and build this software *cough cough*.) Much of it consists of integrating external libraries, most of which are in C++ and using their useful features along with an efficient component system. It currently runs on Linux and Android. We can expect Windows, OSX, and iOS support quite soon. As a design goal, the legalese is designed to be extremely liberal (Apache 2.0.)

Still WIP. Not yet recommended for use. Please feel free to bookmark this repo and come by later because this thing is living with me and awesome things will soon come out of it. :)

Documentation is rather sparse at the moment, and consists of code comments, this page, and issues on the tracker. The issues are well-written, thoughtfully labeled, and organized into the right milestones. Link:

https://github.com/ColinGilbert/noobwerkz-engine/milestones

These can provide a sense of direction as to the direction of the project; I encourage you to browse in there (or the source code.)

High-quality tutorials and real unit tests will come when the implementation is more or less set in stone.

Current version: Version 0.2.5: "Deboostify feat. Graphix Magix, Act Two"

Specific features:
```
A highly tunable gradient mapping shader by default. Comes with three XY-tileable textures blended into one.
Triplanar shader for non-UV mapped objects. Texture reads are kept to a minimum (3) by using the gradient mapping technique.
Provides clean interfaces over to those libraries and tools on top via both a scripting API for quick scene setups or a C++ level API for faster runtimes (or) extension.
```

Currently brings in (among others):
```
BGFX
Bullet Physics
PolyVox
AngelScript
```

To clone:
```
git clone --recursive https://github.com/ColinGilbert/noobwerkz-engine.git
```

Structure:
```
The current setup uses a platform-specific starter program to launch the main application.

There are many libraries - mostly utility libraries - in the engine/lib directory. Not all of them are being used. This needs a cleanup, but at the moment this is still my personal dev repo. Will modularize out soon.

Most engine code is written in engine/common directory, with user-defined functionality linked-in from the sandbox directory. I know it's not correct, but in the current context it's fine.

Short scripts with tiny names are used to do routine work (compiles, archivals, cleanups.) Will eventually bring it into C++ and AngelScript.
```

Platforms:
```
Current setup uses linux as main controller.
Designed so as to be stupidly easy to port (set up your platform-specific code and define an entry point and we're good to go!)
```

Little *nix scripts (currently runs on bash but I don't believe it uses any bashisms:
```
sandbox/b for build. Attempts to build source
sandbox/bb for big build. Wipes the binaries and builds from scratch
sandbox/s for shaders. Builds the full set of shaders from sandbox/shader_src and places them into the sandbox/shaders directory
sandbox/g for git. Adds changes to git and commits them (for my use, mostly)
```

Future:
```
Scripting (done!)
Sound
Serialization
Ports to Android, iOS, Windows, OSX, Emscripten, Steam
Basic AI
Destructible worlds
Test framework
```
