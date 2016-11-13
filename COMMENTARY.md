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
