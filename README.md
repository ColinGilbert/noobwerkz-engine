Hi,


I am developing a solid, fast, easy-to-use cross-platform game engine. This toolkit is made specifically to reduce wasted time (*cough cough* aside from the time it took to design and build this software *cough cough*.) Much of it consists of integrating external libraries, most of which are in C++ and using their useful features along with an efficient component system. Work is also underway on homegrown tools. It currently runs on Linux. We can expect Android, Windows, OSX, and iOS support quite soon. Legalese is quite liberal (Apache 2.0.)

Still WIP. Not yet recommended for day-to-day use. Please feel free to bookmark this repo and come by later because this thing is living with me and awesome things will soon come out of it. :)

Documentation is rather sparse at the moment, and consists of code comments, this page, and issues on the tracker. The issues are well-written, thoughtfully labeled, and organized into the right milestones. Link:

https://github.com/ColinGilbert/noobwerkz-engine/milestones

These can provide a broad sense as to the direction of the project; I encourage you to browse in there (or even better, the source code itself.)

High-quality tutorials and real tests will come when the implementation is more or less mature.

Current versions:
```
Master: 0.5 "StageCraft"
Dev: 0.6 "AI and More!"
```

Specific features:
```
A highly tunable gradient mapping shader by default. Comes with three XY-tileable textures blended into one.
Triplanar shader for non-UV mapped objects. Texture reads are kept to a minimum (3) by using the gradient mapping technique.
Everything created from convex shapes.
```

Currently brings in (among others):
```
BGFX
Bullet Physics
libogg/libvorbis (soon Opus)
r8brain's excellent resampling routines
```

To bootstrap:
```
Currently testing our bootstrap scripts :)
```

Structure:
```
The current setup uses a platform-specific starter program to launch the main application.

There are many libraries - mostly utility libraries - in the engine/lib directory.

Most engine code is written in engine/common directory, with user-defined functionality linked-in from the sandbox directory. I know it's not correct, but in the current context it's fine.

There are also a few smaller libraries (mostly those I wrote) that are pulled in via script (since git submodules suck.)

Short scripts with tiny names are used to do routine work (compiles, archivals, cleanups.)
```

Platforms:
```
Current setup uses linux as main controller.
Designed so as to be stupidly easy to port: Set up your platform-specific bootstrap code, define entry points, compile your shaders (soon unnecessary) and we're good to go!
Ports coming soon.
```

Little *nix scripts (currently runs on bash but I don't believe it uses any bashisms:
```
sandbox/b for build. Attempts to build source
sandbox/bb for big build. Wipes the binaries and builds from scratch
sandbox/s for shaders. Builds the full set of shaders from sandbox/shader_src and places them into the sandbox/shaders directory
sandbox/g for git. Adds changes to git and commits them (for my use, mostly)
sandbox/p for pull. Pulls in remotes that I am often changing.
```

Future:
```
Serialization
Ports to Android, iOS, Windows, OSX, Emscripten, Steam
Basic AI
Destructible worlds
Test framework
```
