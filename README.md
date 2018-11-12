# dhewm3 Mod SDK

This repository contains an SDK that can be used to create modifications ("mods")
for (or port Doom3 mods to) [dhewm3](https://dhewm3.org).

It contains (mostly) the same source files as the original Doom3 SDK, but these
are taken from dhewm3 and **are licensed under GPLv3, not the SDK license**.  
Another small difference is that this is built using [CMake](https://cmake.org/)
instead of SCons + VS Project files.  
This means that you need CMake to build it, but don't worry, on Windows it can
create a Visual Studio Solution for you so you can program and compile with
Visual Studio like you might be used to.

## Some ports of existing Mods

This repository also contains ports of existing mods whichs authors released
the source under GPL; you can find these in their own branches:
[Classic Doom 3](https://github.com/dhewm/dhewm3-sdk/tree/cdoom) and
[Denton's Enhanced Doom3](https://github.com/dhewm/dhewm3-sdk/tree/dentonmod) and
[HardQore2](https://github.com/dhewm/dhewm3-sdk/tree/hardqore2)

You can find Win32 DLLs of those mods that work with dhewm3 1.5.0RC1 (and newer) at https://github.com/dhewm/dhewm3/releases/download/1.5.0_RC1/dhewm3_1.5.0_RC1_win32.zip

## How to build

(I should eventually expand this, especially for Windows)

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
