# HeXen: Edge Of Chaos

This is the source of HeXen: Edge Of Chaos (aka HEOC) game by HEOC Team, LegendGuard ported to the dhewm3 SDK.

The original source can be downloaded at https://www.moddb.com/mods/hexen-edge-of-chaos/downloads/hexen-edge-of-chaos-demo-source-patch
or https://github.com/LegendaryGuard/HeXen_Edge_Of_Chaos (Special thanks: LegendGuard).

Reference (for more info): https://web.archive.org/web/20190803175542/http://hexenmod.net/about/

heXen:Edge of Chaos will be a new and free game, based on the original Hexen franchise that was developed by id Software and Raven Software. The game utilizes the id Tech 4 GPL engine that id Software released on November 22, 2011. Previously the project was a total conversion for Doom 3.

Edge of Chaos is being developed by a team of fans who have always loved the original Hexen since it came out. In fact, most of us spent countless hours with the entire series. When id Software released Doom3, we immediately saw immense potential in the engine itself; the 'Hell' maps gave us the inspiration to make a hack-and-slash game based, primarily, on the original Hexen. Our story ties into the expansion packs and also touches upon the Heretic narrative; especially the incredible atmosphere painted into these rich worlds and their darkling journeys.

The game will be released in three episodes, each based on one character class. Our first demo release, an excerpt from the first episode, is based on the Cleric class. The second and third episodes will be based on the warrior and sorceress class respectively. We've chosen to use an episodic approach in order to create a manageable release schedule.

You can download the mod resources at https://www.moddb.com/games/doom-iii/downloads/hexen-edge-of-chaos-dhewm3-edition

You could start it with `./dhewm3 +set fs_game eoc`

## How to build

### On Windows

You need [CMake](https://cmake.org/) either Visual Studio (2010 and newer have been tested) or [MinGW-w64](https://mingw-w64.org/)

1. Clone the dhewm3-sdk git repo
2. switch to your git clone's directory: `cd dhewm3-sdk`
2. (optional: switch to an existing mods branch: `git checkout hexeneoc`)
3. create a **build directory** in your `dhewm3-sdk/` directory (`build/` or `build-eoc/` or whatever)
4. Start the CMake GUI
5. Select your `dhewm3-sdk/` folder for *"Where is the source code"* and your  
   **build directory** from step 3 for *"Where to build the binaries"*.
6. Click `[Configure]`, select what you want to build with, e.g. "Visual Studio 15 2017", and the platform to target,
   probably "**Win32**" (for compatibility with the official dhewm3 Windows binaries), click `[Finish]`, wait for CMake to do its thing
   - If you're using **MinGW** you'll have to select your build type now, e.g. `Debug` (not optimized but debuggable) or `Release` (optimized and thus faster, but can't be debugged that well).
   - (For Visual Studio you don't have to select a build type now, you  can do it in in Visual Studio)
7. Click `[Generate]`
8. Building:
   * If you're using **Visual Studio**, you should be able to just click `[Open Project]` to open the generated Project in Visual Studio. You can now compile the SDK in Visual Studio (and of course make your changes to the code).
   * (Untested:) For **MinGW**, open your MinGW or MSys shell, switch to your **build directory** and execute `make -j4` to build the game DLL
9. Now it's time to copy the DLL (`eoc.dll`) to your dhewm3 install, where eoc.dll and d3xp.dll are
   - For **Visual Studio** the DLL should be in a subdirectory of your **build directory**, depending on the build type you selected in `build/Debug/` or `build/Release/` or similar
   - For **MinGW** the DLL should be directly in your **build directory**.
10. start the game with that mod, like `dhewm3 +set fs_game eoc`

(Make sure to actually have the mods game data in the right directory as well;
the directory name should be the same as the game lib name, but without .dll,
for example dentonmod/ for dentonmod.dll)

### On Linux and other Unix-likes

On Linux and similar the following should work (if you have cmake, make and GCC/g++ installed):
1. Clone the dhewm3-sdk git repo
2. switch to your git clone's directory: `cd dhewm3-sdk`
3. (optional: switch to an existing mods branch: `git checkout hexeneoc`)
4. create a build directory: `mkdir build`
5. switch to build directory: `cd build`
6. create Makefile with CMake: `cmake ..`
   - by default, this will create an *RelWithDebInfo* build, which is optimized but still has debug info, so it's somewhat debuggable. You can select another kind of build with `cmake -DCMAKE_BUILD_TYPE=Debug ..` for a Debug build with less optimization, which will make it easier to debug (but possibly slower). You could also replace "Debug" with "Release" for a proper optimized Release build without any Debug info.
7. compile the mod .so: `make -j4`
8. it (e.g. `eoc.so`) should now be in the build/ directory,
   copy it to your dhewm3 install, where eoc.so and d3xp.so are
9. start the game with that mod, like `dhewm3 +set fs_game eoc`

(Make sure to actually have the mods game data in the right directory as well;
the directory name should be the same as the game lib name, but without .so/.dylib,
for example dentonmod/ for dentonmod.so)
