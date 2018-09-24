# dhewm3 Mod SDK For Scarlet Rivensin: The Ruiner
Websites:
https://www.moddb.com/mods/ruiner
https://rivensin.tumblr.com/

This repository contains an SDK that can be used to create the Rivensin modification ("mod")
for [dhewm3](https://dhewm3.org).

It contains (mostly) the same source files as the original Doom3 SDK, but these
are taken from dhewm3 and **are licensed under GPLv3, not the SDK license**.  
Another small difference is that this is built using [CMake](https://cmake.org/)
instead of SCons + VS Project files.  
This means that you need CMake to build it, but don't worry, on Windows it can
create a Visual Studio Solution for you so you can program and compile with
Visual Studio like you might be used to.

Over a dozen plus files are different from the original sdk.  Some features include:
+improvements to thirdperson camera
+Improved collision based melee combat system
+"special move" style inputs for player attacks similar to fighting games
+Movement Directional based attacks
+Dashing, dodging and wall jumping using the the new input system mentioned above.
+trigger Full body animations for player model
+Increase damage of attacks during animations frames
+Updates to using ammo and sharing ammo across weapons
+Proper third person crosshair for Doom3.  
+Head shot system for enemies.
+Player can cast a shield around themselves which uses the old stamina gauge system
+HDR lighting with bloom and shift sensitvity. This is a modified version of Denton's HDR system.
+Improved ambient lighting using a modified Rebb's ambient lights shader.  However this is not apart of the sdk and part of the game itself.
+Doom3: ROE grabber "gravity gun"
+Dynamic spread for weapons based upon whether the player is moving, croushing, running and walking
+Walk aim mechanic which also triggers the camera to zoom in while active
+Interact "use" key
+High Pain "stunning" for enemies.
+Per weapon based zooming
+Many improvements from the Doom 3 Denton mod, which is on Dhewm 3.
      -Attachment system for lights and particles to weapons 
      -New ultra quality yet highly scalable(DX9+ only) bloom shader with HDR like shift sensitivity.
      -Material based decals, particles and sound effects.
      -New blood splat and blood pool decals system
      -Enemies can leave behind trails of blood when damaged.
      -Improved behavior of exploding and burning barrels
      -Lots of bug fixes, improvements and optimizations
      -All new tracer logic for creating tracers and beams
      -Support for up to 32 weapons with shared ammo considerations.
      -Half-Life style quick weapon key.
      -Improved Gibbing system.
      -Tweaked ragdolls react to explosions, now an explosion can throw animated bodies in air.
      -Physics based collision sounds for debris.
      -Many improvements for Damage feedback.
      
Please note that this is a thirdperson modification for Dhewm 3.  Many first person features have been disabled or commented out of the sdk and also in the game's assets and scripts.  Many first person models will not physically appear as well.  Most vanilla Doom 3 campaign levels will not load in the mod.  However with the 2018 update custom maps are supported.  It is possible to use sdk and apply many of the features added to a first person mod.

## How to build

(I just eventually expand this, especially for Windows)

On Linux and similar the following should work (if you have cmake, make and GCC/g++ installed):
1. Clone the dhewm3-sdk git repo
2. switch to your git clone's directory: `cd dhewm3-sdk`
3. (optional: switch to an existing mods branch: `git checkout dentonmod`)
4. create a build directory: `mkdir build`
5. switch to build directory: `cd build`
6. create Makefile with CMake: `cmake ..`
7. compile the mod .so: `make -j4`
8. it (e.g. `dentonmod.so`) should now be in the build/ directory,
   copy it to your dhewm3 install, where base.so and d3xp.so are
9. start the game with that mod, like `dhewm3 +set fs_game dentonmod`

(Make sure to actually have the mods game data in the right directory as well;
the directory name should be the same as the game lib name, but without .so/.dylib/.dll,
for example dentonmod/ for dentonmod.so)

On Windows it should be kinda similar, but you have to tell CMake to create
a Visual Studio solution for the VS version you're using, or Makefiles for
MinGW-w64 or whatever.

## How to port a Mod to dhewm3

The usual (easiest) way to port a mod is to make a diff between the mod's source
and the Doom3 SDK and apply the resulting patch to the vanilla game source (from the master branch).  
Afterwards usually some manual work must be done to resolve patching conflicts and get the mod to compile.  
Also, the CMakeLists.txt file must be adjusted (see the dentonmod branch for examples).

Please note that currently I only accept mods that are released under the
GPL license - the one used by Open Source Doom3 (i.e. *not* only the Doom3 SDK license) -
because neither the GPL nor the SDK license allow merging code from both licenses.  
So please get permission from the mod authors first.

Yes, this unfortunately means that unless you manage to contact Sikkpin and get
his permission, there will be no Sikkmod for dhewm3 (and neither other mods that
use Sikkmod code) :-(  
*(If you are Sikkpin, please get in touch!)*

### Getting in touch

If you are a mod author and want to release your mod's sourcecode under GPL,
but don't want to port it yourself (or don't have time or are unsure how)
please contact me, I can probably help you :-)

The easiest way to contact me is by creating an issue in this Github repository,
or by sending a DM to *caedes* in the [id Tech Forums](http://idtechforums.fuzzylogicinc.com/)
or by pinging *caedes* in the #iodoom3 IRC channel on FreeNode.
