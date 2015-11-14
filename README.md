I'm developing a rather minimalistic cross-platform game/simulation engine. This toolkit is made specifically to reduce wasted time (*cough cough* aside from the time it took to design and build this software *cough cough*.) Much of it consists of integrating external libraries, most of which are in C++11. It currently runs on Linux and Android. We can expect Windows, Mac, and iOS support quite soon. As a design goal, the legalese is designed to be extremely liberal (Apache 2.0.)

Still WIP.


Specific features:
```
A highly tunable gradient mapping shader by default. Comes with three XY-tileable ntextures blended into one.
Triplanar shader for non-UV mapped objects. Texture reads are kept to a minimum (3) by using the gradient mapping technique.
No significant obstacles between the programmer, the lower-level libraries and the data.
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
```

To clone:
```
git clone --recursive https://github.com/ColinGilbert/noobwerkz-engine.git
```

Program structure:
```
The current setup uses a platform-specific starter program to launch the main application.

There are many libraries - mostly math and scientific libraries - in the engine/lib directory.

Most engine code written in engine/common directory.

```

Platforms:
```
Current setup uses linux as main controller, but overall aim is awesome devops. Future goals include adding configs + binary packages (likely Xen+Gentoo Hardened or Exherbo) for: Very secure virtualization system config, multimedia workstation, network filtering (with Bro IDS + others), network gateway/v-switch node with secure and well-configured management, wifi controller node, keyserver node, secure terminal, and fileserver node (but that last one is very likely to be based on FreeBSD-Hardened.) Will also include middleware for a using proper, dedicated OpenBSD firewall. Goal is to enable small-medium (and even large) businesses on tight integration without sacrificing flexibility and moddability. Focus is for people wanting to set up a proper lab, with a specific minimum hardware investment being the established, proper norm. Will also include support for interfacing with Windows and Apple ecosystems. I intend on obtaining the LPIC-3's, an MCSA (or two), the CCNP, and the CWNP certs so this investement of time and effort suits me well and might benefit others some day. 

```

Readiness
``
Not yet recommended for use. See later.
``

Documentation:
```
Documentation is rather sparse at the moment, but this is still a fledgeling endeavour and my plans are to begin really working on it during the winter months. More progress will be coming soon.
For the future, I plan on continuing work on it quite vigorously.
```

Things to do in the immediate future:
```
Continue refining the simulation world.
AI
Sound
Serialization
```

Medium future
```
Ports to Win, MacOS, iOS, Emscripten, NaCL
Test framework
Destructible worlds
```

Long-term
```
Dataviz
Cutting edge/esoteric programming idioms
```
