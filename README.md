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
[Classic Doom 3](https://github.com/dhewm/dhewm3-sdk/tree/cdoom),
[Denton's Enhanced Doom3](https://github.com/dhewm/dhewm3-sdk/tree/dentonmod),
[Fitz Packerton](https://github.com/dhewm/dhewm3-sdk/tree/fitz)
[HardQore2](https://github.com/dhewm/dhewm3-sdk/tree/hardqore2) and
[Scarlet **Rivensin**: The Ruiner](https://github.com/dhewm/dhewm3-sdk/tree/rivensin)

You can find Win32 DLLs of those mods that work with dhewm3 1.5.0 at https://github.com/dhewm/dhewm3/releases/download/1.5.0/dhewm3-mods_1.5.0_win32.zip  
The equivalent Linux amd64 (x86_64) libs can be downloaded at https://github.com/dhewm/dhewm3/releases/download/1.5.0/dhewm3-mods_1.5.0_Linux_amd64.tar.gz

## How to build

### On Windows

You need [CMake](https://cmake.org/) either Visual Studio (2010 and newer have been tested) or [MinGW-w64](https://mingw-w64.org/)

1. Clone the dhewm3-sdk git repo
2. (optional: switch to an existing mods branch: `git checkout dentonmod`)
3. create a **build directory** in your `dhewm3-sdk/` directory (`build/` or `build-dentonmod/` or whatever)
4. Start the CMake GUI
5. Select your `dhewm3-sdk/` folder for *"Where is the source code"* and your  
   **build directory** from step 3 for *"Where to build the binaries"*.
6. Click `[Configure]`, select what you want to build with, e.g. "Visual Studio 15 2017", click `[Finish]`, wait for CMake to do its thing
   - If you're using **MinGW** you'll have to select your build type now, e.g. `Debug` (not optimized but debuggable) or `Release` (optimized and thus faster, but can't be debugged that well).
   - (For Visual Studio you don't have to select a build type now, you  can do it in in Visual Studio)
7. Click `[Generate]`
8. Building:
   * If you're using **Visual Studio**, you should be able to just click `[Open Project]` to open the generated Project in Visual Studio. You can now compile the SDK in Visual Studio (and of course make your changes to the code).
   * (Untested:) For **MinGW**, open your MinGW or MSys shell, switch to your **build directory** and execute `make -j4` to build the game DLL
9. Now it's time to copy the DLL (e.g. `dentonmod.dll`) to your dhewm3 install, where base.dll and d3xp.dll are
   - For **Visual Studio** the DLL should be in a subdirectory of your **build directory**, depending on the build type you selected in `build/Debug/` or `build/Release/` or similar
   - For **MinGW** the DLL should be directly in your **build directory**.
10. start the game with that mod, like `dhewm3 +set fs_game dentonmod`

(Make sure to actually have the mods game data in the right directory as well;
the directory name should be the same as the game lib name, but without .dll,
for example dentonmod/ for dentonmod.dll)

### On Linux and other Unix-likes

On Linux and similar the following should work (if you have cmake, make and GCC/g++ installed):
1. Clone the dhewm3-sdk git repo
2. switch to your git clone's directory: `cd dhewm3-sdk`
3. (optional: switch to an existing mods branch: `git checkout dentonmod`)
4. create a build directory: `mkdir build`
5. switch to build directory: `cd build`
6. create Makefile with CMake: `cmake ..`
   - by default, this will create an *RelWithDebInfo* build, which is optimized but still has debug info, so it's somewhat debuggable. You can select another kind of build with `cmake -DCMAKE_BUILD_TYPE=Debug ..` for a Debug build with less optimization, which will make it easier to debug (but possibly slower). You could also replace "Debug" with "Release" for a proper optimized Release build without any Debug info.
7. compile the mod .so: `make -j4`
8. it (e.g. `dentonmod.so`) should now be in the build/ directory,
   copy it to your dhewm3 install, where base.so and d3xp.so are
9. start the game with that mod, like `dhewm3 +set fs_game dentonmod`

(Make sure to actually have the mods game data in the right directory as well;
the directory name should be the same as the game lib name, but without .so/.dylib,
for example dentonmod/ for dentonmod.so)

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
If you prefer E-Mail, you can find my address in the [git commits](https://github.com/dhewm/dhewm3-sdk/commit/b7d77c468a42892fa3c03a9ce0683916a110e8db.patch).

## New features that mods can use

dhewm3 has some features that the original Doom3 didn't have that are interesting for Mods.

### Injecting all supported resolutions into the video menu

Mods that have their own video settings menu can tell dhewm3 to replace
the "choices" and "values" entries in their choiceDef with the
resolutions supported by dhewm3 (and corresponding modes).  
So if we add new video modes to dhewm3, they'll automatically appear in
the menu without changing the .gui  
To enable this, you only need to add a `injectResolutions 1`
entry to the resolution choiceDef. By default, the first entry will
be "r_custom*" for r_mode -1, which means "custom resolution, use
r_customWidth and r_customHeight".  
If the "r_custom*" entry should be disabled for your mod, just add another entry:
`injectCustomResolutionMode 0`

### Scale GUIs to 4:3

Fullscreen menus (like the main menu and the PDA) are scaled to 4:3 by default, adding black bars on the left/right when using widescreen resolutions (users can disable this with `r_scaleMenusTo43 0`).  
By default, this is not done for other GUIs, mainly because the HUD is a fullscreen GUI and also handles damage effects (coloring the whole screen red), which would look shitty if scaled to 4:3 with black/empty bars on the left/right.  
However, you can still enable this for your WIN_DESKTOP GUIs, either in the .gui itself or via C++ code when loading the GUI (I found this especially useful for the crosshairs/cursor GUI).

WIN_DESKTOP means that this can currently only be set for the top-level window in a .gui (all its subwindows/widgets will be scaled implicitly).

There are two ways to make a GUI use this:
1. in the **.gui** add a window variable `scaleto43 1`, like
   ```
	windowDef Desktop {
	rect	0 ,0 ,640 ,480
	nocursor	1
	float	talk 	0

	scaleto43 1

	// .. etc rest of windowDef
   ```

2. When creating the GUI from **C++ code**, you can afterwards make the
   UserInterface scale to 4:3 like this:
   ```c++
    idUserInterface* ui = Whatever(); // create it
    ui->SetStateBool("scaleto43", true);
    ui->StateChanged(gameLocal.time);
   ```
   Both lines are important!  
   
   Keep in mind that if the GUI is saved to the savegame, you need to call this after restoring the GUI from the savegame,
   see https://github.com/dhewm/dhewm3-sdk/commit/5070b8c7ec6f3a8ba1cb4123de37732f9cd9437f for an example.
   
   Also note that you can *not* generally inject variables into GUIs like that from C++, dhewm3 has special code to make the `"scaleto43"` case work.
