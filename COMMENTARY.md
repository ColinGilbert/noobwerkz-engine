Jan 17, 2017
```
Been working in SQLite a lot. The database schema is more or less complete and all that needs implementing is going to be the bodies and shapes. Overall, we're 7/8 of the way before writing test-cases. Along the way, I decided to re-implement the mesh data structure in order to have it not crash. It will soon be a proper class with an interface wrapped around it.```
```


Jan. 9, 2017
```
Merge to master. Been starting on the editor tools themselves.
```

Dec. 30, 2016
```
Harfbuzz caused trouble. Went back to Freetype. Aside from kerning and atlas padding, we're good.
```

Dec. 18th, 2016
```
Been reading up a lot on Harfbuzz, ICU, and Unicode last week. In fact, it's been far too much reading about all that... I've almost finished implementing text-shaping with Harfbuzz, along with an attendant refactoring of the graphics API. ICU will be for later, since it's huge and rather difficult to embed.
```

Dec 11th, 2016
```
Yesterday, the terrain shader finally worked. Today, I templated all the math functions. That part of a bit painful but now we're far better off.
```

Dec 9th, 2016
```
Done with GL textures. Also been working on (and now almost done) bringing over the terrain shader to GLSL.
```

Dec 2, 2016
```
Graphics API now makes sense. Now porting texture GLAPI.
Got rid of singleton code bloating globals and graphics classes by replacing it with templates. :)
```

Nov 27, 2016
```
Starting to reimplement textures in OpenGL ES and tweaking API for usability. Glad that the sound problems seem to be behind us.
```

Nov 25, 2016
```
Sound seems to work mostly well enough on Android. We may add a deeper buffer to account for trouble devices and add better error reporting, but the basics are in. Hurray.
```

Nov 24, 2016
```
Fixed the Android sound troubles that were driving me insane this past week! Turns out it was a subtle multiplication and division order bug that caused lots of grief, but only once in a while on certain ARM chipsets. Will be looking into better methods of dealing with such horrors. Either way, this thing isn't a problem anymore and we're good to go. Phew...
```

Nov 22, 2016
```
Been debugging Android Audio a lot. After much grief and head-banging, I have been able to output test audio by explicitly filling a large buffer and offering chunks of it to the callback. Said callback is WAY more painful than I anticipated, but I think that by rendering audio asynchronously we should hopefully do nicely.
```

Nov 16, 2016
```
Sanitary interface between Bullet and rest of API is now set in properly. Newest release seems to have fixed the unpredictable crashes, while a bug on Android was solved by removing -ftrapv compiler flags. AKA: Found a numeric bug in Bullet. :) This officially means that we can retain a Bullet compatibility layer as benchmark against our own algorithms. :)

After fighting with a confusing (shader-caused, durr) bug, we cut our draw time in half by using glMapBufferRange instead of glBufferSubData.
```

Nov. 12, 2016
```
Had to debug troublesome bug on secondary testing device: Tried switching to C++ EGL and got same problems. Switched back to Java-based and managed to update phone which fixed bug. :P
Otherwise, crash bugs in Bullet are forcing a change of physics API (currently taken out) along the same lines of our current graphics API.It may be due to the fact that I changed the size of the btCollisionBody by putting in uint32 indices (and taking out void pointers),which may be harming the alignment. Either way, it's a good excuse to make our physics API hide the underlying library in a private implementation and tonight marks the night where we start doing so.
Also been reading "Realtime Collisio Detection" by Christer Ericson and beefing up strongly on linear algebra, game physics, and graphics programming. Got more good books on order to help with that.
```

Oct. 29, 2016
```
Finally got all bits for OpenGL graphics backed up by a seamless API.
```

Oct 26, 2016
```
Been cleaning up the codebase a lot this month. Most project time since has been spent on cleaning up the engine, learning OpenGL, and rethinking design to better take into account the new way things are being done while being as fast and readable as possible.
```

Oct 6, 2016
```
Begin proper OpenGL integration by removing bgfx.
```

Oct 4, 2016
```
Tried to make bgfx work by updating shaders in an effort to not be forced to refactor all graphics, but no go. :(
Continued to refactor graphics API.
Switched to Clang and refactored build flags in order to root out bugs. Found some.
```

NOTE: The development process stretched far beyond the initial date on this document, as keeping notes is a habit I have decided to re-learn.
