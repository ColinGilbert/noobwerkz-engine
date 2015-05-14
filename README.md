NOTICE: This project is still early-phase. Specifically, integration plan is still rather shaky and over half of the targeted build systems aren't yet in place. Starting a project now _might_ break code later and require refactoring on the end-user's part. However, if you keep your code within your own directory in sandbox directory, you should be able to avoid a merge conflict.


I'm developing a cross-platform game/simulation/data analysis engine with pretty front-ends. This toolkit is made specifically to reduce wasted time and to encourage code reuse in (what I hope) is an elegant way. It is written mostly in C++ and currently runs on Linux and Android. We can expect Windows, Mac, and iOS support within the month. I expect to spend a good portion of my waking life (and a some of my dreams) in it, so code quality is held at a high standard. This home has the lights on and is being fixed up and vacuumed.

The current focus is on enabling games and simulations. It's still quite young and stupid, but it will eventually contain lots of useful and fast-running C/C++ libraries bundled up in a usable way (especially with regards to games/simulations), with easy access to the underlying data. Due to the early nature of development, documentation is practically non-existent. However, the function names and variables hopefully make sense and should be free of typos, and I do often enough sometimes add explanations for wtf's. However, I try to keep those to a bare minimum. :)

It integrates the more-than-excellent https://github.com/bkaradzic/bgfx library to provide cross-platform graphics. Currently uses CMake (although Premake/Genie look better on many levels, and I'm up to write my own build system but only if its going to be worthwhile.)

As for its future, I plan on continuing work on it quite vigorously. It should be able to port multimedia to the browser by mid-summer and should easily enough be made to do stuff as a console app. Currently the focus is on integrating these cool things as a fast-running mobile environment with a more featureful (due to the difficulty of porting libs) desktop environment. The desktop currently focuses on Linux but I will be porting to Windows, Mac, and iOS soon enough. I am currently looking for a simple, fast, web server with a palatable license to integrate. I'll add a constraint solver and statistical functions probably by September. As a design goal, the legalese is intended to be extremely liberal.


Program structure:

The current setup uses a platform-specific starter program to launch the main application. For desktop, www.glfw.org is used as its non-obtrusive and works nicely on my main environment, which happens to be Linux, but it should port to Mac and Windows nicely. However, I'm up for writing more entrypoints if doing so proves desirable. The rest is graphics management code, much of it delegated to other libraries such as Assimp, the previously-mentioned bgfx, and soon enough some other mesh libraries. THere is a bit of scoping/memory-management glue code, but its very light and focuses on gettings things to work without great obstruction and rapid extendability. Compile times on my system are fast, due to careful header management and (mostly) CMake caching goodness. The rest of the libraries are available within the usual namespaces and there is a network logger powered by Asio (https://think-async.com/) that I use constantly.



Things to do in the immediate future:
```
Setup more hardware environments (Win, MacOS, iOS)
Add:
	PolyVox (http://www.volumesoffun.com/polyvox-about/)
	Noise++
	Mesh CSG

```

Stuff to find/make:
```
Skeletal animation/IK code
Statistics library
Constraint solvers - (Gecode?)
Graph library (Lemon?)
```

Long-term
```
rbdl(?)


```
