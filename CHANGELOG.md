CHANGELOG:


Version 0.2.1 AKA: "iOS port + Graphics Magix, Act Two" (in progress)
```
Port to iOS (in progress.)
Added random number function.
Fix lots of nasty bugs, major repo cleanup.
Add convenient actor-adding functions.
Add lighting.```
```

Version 0.2.0 AKA: "Graphix Magix, Act One"
```
Overhaul of the graphics system. Now allows for (eventual) easy batching of instanced objects using a very compact scene graph (it only holds tags stored in std::vector containers.)
Used to store extra meshes for no good reason. This stupidity was stopped and we now save lots of memory. :P
```

Version 0.1.1 AKA "Live reloads"
```
Moved out all non-stage components from stage into globals, enabling live reloads faster! Yay!
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
