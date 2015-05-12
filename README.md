If you're looking to make cross-platform games and simulations, my own framework - warning, rather shamelessly-biased opinions ahead - www.github.com/ColinGilbert/noobwerkz-engine is integrating the best open-source libraries in (what I hope) is a good way to get bootstrapped. It contains the wisdom learnt from tieing together a lot of stuff and getting burnt repeatedly. The current focus is on enabling games and simulations. It's still quite young and stupid, but it will eventually contain lots of useful and fast-running C/C++ libraries bundled up in a usable way (especially with regards to games/simulations), with easy access to the underlying data. Due to the early nature of development, documentation is practically non-existent. However, the function names and variables hopefully make sense and should be free of typos, and I do often enough sometimes add explanations for wtf's. However, I try to keep those to a bare minimum. :)

It integrates the more-than-excellent https://github.com/bkaradzic/bgfx library to provide cross-platform graphics. Currently uses CMake (although Premake/Genie look better on many levels, and I'm up to write my own build system but only if its going to be worthwhile.)

As for its future, I plan on continuing work on it quite vigorously. It should be able to port multimedia to the browser by mid-summer and should easily enough be made to do stuff as a console app. Currently the focus is on integrating these cool things as a fast-running mobile environment with a more featureful (due to the difficulty of porting libs) desktop environment. The desktop currently focuses on Linux but I will be porting to Windows, Mac, and iOS soon enough. I am currently looking for a simple, fast, web server with a palatable license to integrate. I'll add a constraint solver and statistical functions probably by September. As a design goal, the legalese is intended to be extremely liberal.

Program structure:

I intend on reshuffling directories soon, so please don't rely on that yet.

The current setup uses a platform-specific starter program to launch the main application. For desktop, www.glfw.org is used as its non-obtrusive and works nicely on my main environment, which happens to be Linux (go Gentoo!) but it should port to Mac and Windows nicely. However, I'm up for writing more entrypoints if doing so proves desirable. The rest is graphics management code, much of it delegated to other libraries such as Assimp, the previously-mentionned bgfx, and soon enough some other mesh libraries. THere is a bit of scoping/memory-management glue code, but its very light and focuses on gettings things to work without great obstruction and rapid extendability. Compile times on my system are fast, due to careful header management and (mostly) CMake caching goodness. The rest of the libraries are available within the usual namespaces and there is a network logger powered by Asio (https://think-async.com/) that I use constantly.

Libraries to integrate in the immediate future:
```
PolyVox (http://www.volumesoffun.com/polyvox-about/)
```

Stuff to find:
```
Skeletal animation of some kind
Statistical library (preferaby header-only)
```
