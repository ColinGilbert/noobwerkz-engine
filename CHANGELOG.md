CHANGELOG:

Master: 0.7 "Oh Dear... Droids!"
```
Android port! Setting up the sound was even crummier than expected, but now it works!
Replaced BGFX entirely.
Graphics and physics are now opaque interfaces returnng handles and the occasional buffer (to be used responsibly.) Sound API to follow.
More major cleanups
```

Version 0.6 AKA: "Cleaning House!"
```
Additions:
Profiling
Stub actor control loop.


Bugfixes/Cleanups:
NO MORE RTTI/EXCEPTIONS! JUST LIKE A REAL GAME ENGINE!
Templatize math types.
Protect constant globals and graphics members.
Removed many unused libraries from libs folder.
Removed AngelScript, OpenMesh, PolyVox, fmtlib and Assimp from codebase (Assimp is now build-time optional as an editor tool.)
Remove lemon-graph and replace with homegrown.

Overall, the executable size is trimmed to a little under half of what it used to be.

Notes:

Tried and failed to use recast/detour
Ran into showstopper while trying to port to Android. Tried literally everything. Posted bug report and if we don't soon get it fixed we'll have to redo graphics entirely.
```


Version 0.5 AKA: "StageCraft"
```
Additions:
Implemented actors, cleanly.
Add joints to scripting.

Bugfixes/Cleanups:
Remove git modules as they were causing problems
Make graphics class non-static
Banish use of -fpermissive from entire codebase
```


Version 0.4 AKA: "Auditory Delights"
```
Sound works properly
Engine finds native audio rate and resamples sound effects to it upon load.
Engine always plays audio at native rate.
```


Version 0.3 AKA: "Smorgasbord of Delights"
```
Began adding sound
Performance enhancements
Cleaned up components system
Created graph class (without exceptions!) to implement our own algorithms. Will eventually replace more cumbersome third-party libs that link in lots of extra binary and force exceptions onto the whole project.
Cleanups all over :P
```

Version 0.2.5: "Deboostify feat. Graphix Magix, Act Two"
```
Removed all calls to Boost in response to horrible porting experiences.
Added RNG function
Lighting
Repo cleanup
Nice API for scripts and C++
```

Version 0.2.0 AKA: "Graphix Magix, Act One"
```
Overhaul of the graphics system. Now allows for (eventual) easy batching of instanced objects using a very compact scene graph (it only holds tags stored in std::vector containers.)
Used to store extra meshes for no good reason. This stupidity was stopped and we now save lots of memory. :P
```

Version 0.1.1 AKA "Live reloads"
```
Moved out all non-stage components from stage into globals, enabling faster live reloads! Yay!
```

Version 0.1 AKA "HAHA we got scripting!"
```
Added proper working scripting and ported example to it. Was major PITA. :)
Minor/medium refactorings/cleanups.
Fixes to the triplanar shader.
```

Version 0.0.2 AKA "Starting to pull through"
```
Lots of cleanups.
Complete overhaul of stage class. Now properly keeps track of items, can compose entities, and has a far cleaner interface!
Fixed mesh loading. Now they do not need an open file on disk.
Default file opening interface that gives you the option to keep file cached in memory.
```

Version 0.0.1.1:
```
Code cleanups.
Issues and milestones done properly.
README.md given overhaul.
```

Version: 0.0.1, AKA: Fresh "beginnings":
```
Beginning of acceptable, almost modular interface.
```
