Hi,


I am developing a solid, fast, easy-to-use cross-platform game and multimedia engine, with a strong emphasis on targeting modern mobile platforms (GLES 3+).

This toolkit is made specifically to reduce wasted programmer time (*cough cough* aside from the time it took to design and build this software *cough cough*.) Much of it consists of integrating external libraries, most of which are in C++ and using their useful features along with an efficient component system to create an efficient single-developer pipeline. It currently runs on Linux and Android. Apple mobile devices, Windows, and OSX support coming quite soon. Legalese is quite liberal (Apache 2.0.)

Still WIP. Not yet recommended for day-to-day use. Please feel free to bookmark this repo and come by later because this is a living project.

If you want to know more about this engine, visit the dev branch; I try keep the master from breaking and so it gets updated less often. More often than not if you see something wrong in master it'll probably have either been tagged or fixed (or removed entirely) in dev. I strive to use intelligent names for things and to never return void pointers unless the hardware forces it; and if that happens I'll either make it typesafe or wrap around it. External libraries are intentionally kept behind tidy interfaces, sometimes even full-on private implementations. The code should therefore be self-explanatory by anyone who is educated in C++. Unavoidable wierdness is noted and transitional code slop that may exist is tagged with a TODO for removal prior to acceptance for the next master branch. Documentation currently consists of code comments, this page, the changelog, a recently-established running commentary in this directory, and issues on the tracker. These can provide a broad sense as to the direction of the project; I encourage you to browse in there (or even better, the source code itself! It's rather nice compared to plenty of other codebases I've seen...) If you have a question, feel free to ask me on the issue tracker! I am a generally nice fellow. :)

High-quality tutorials and real unit/integration tests will come when the implementation is more or less mature.

Current versions:
```
Master: 0.6 "Cleaning House!"
Dev: 0.7 "Android port"
```

Specific features:
```
A highly tunable gradient mapping shader by default. Comes with three XY-tileable textures blended into one.
Triplanar shader for non-UV mapped objects. Texture reads are kept to a minimum (3) by using the gradient mapping technique.
Everything created from convex shapes.
```

Currently brings in (among others):
```
Bullet Physics
libogg/libvorbis (soon Opus)
r8brain's excellent resampling routines
ozz-animation
GLFW (on desktop)
Freetype
Voro++
```

To bootstrap:
```
Currently testing our bootstrap scripts :)
```

Structure:
```
The current setup uses a platform-specific starter program to launch the main application.

There are many libraries - mostly utility libraries - in the engine/lib directory.

Most engine code is written in engine/common directory, with user-defined functionality consisting of a few methods declared in Application.hpp and defined by the user in sandbox/UserApp.cpp. I know it's not correct, but in the current context it allows the user to use low-level information and manipulate the event system directly. It WILL be changed soon so don't expect to use this interface for long (the design is constantly and methodically being reworked for both correctness and lowest burden on the programmer.)

There are also a few smaller libraries (mostly those I wrote) that are pulled in via script (since git submodules can really, really suck.) The rest are brought in as source in the engine/lib directory.

There are short scripts in the sandbox directory that are used to do routine work (compiles, archivals, cleanups.)
```

Platforms:
```
Current setup uses linux as main controller. Also runs on Android.
Designed so as to be stupidly easy to port: Set up your platform-specific bootstrap code, define entry points, and we're good to go!
More ports coming soon.
```


```
Future:
```
Serialization
Ports to iOS, Windows, OSX, and Emscripten
Basic AI
Destructible worlds
Test framework
```
