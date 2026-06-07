# HeXen: Edge Of Chaos

This is the source of HeXen: Edge Of Chaos (HEOC) game by HEOC Team.

You can **download** the required mod **game data** at https://www.moddb.com/games/doom-iii/downloads/hexen-edge-of-chaos-dhewm3-edition

The original source can be downloaded at https://www.moddb.com/mods/hexen-edge-of-chaos/downloads/hexen-edge-of-chaos-demo-source-patch
or https://github.com/LegendaryGuard/HeXen_Edge_Of_Chaos (Special thanks: LegendGuard).

Reference (for more info): https://web.archive.org/web/20190803175542/http://hexenmod.net/about/

HeXen:Edge of Chaos is a free fan game, based on the original Hexen franchise that was developed by id Software and Raven Software. The game utilizes the id Tech 4 GPL engine that id Software released on November 22, 2011. Previously the project was a total conversion for Doom 3.

Edge of Chaos was developed by a team of fans who had always loved the original Hexen since it came out. The team spent countless hours with the entire series. When id Software released Doom3, they saw immense potential in the engine itself; the 'Hell' maps gave them the inspiration to make a hack-and-slash game based, primarily, on the original Hexen. Their story was designed to tie into the expansion packs and also touch upon the Heretic narrative, aiming to capture the incredible atmosphere of those rich worlds and their darkling journeys.

The project was planned for release in three episodes, each based on one character class. The only release, a demo excerpt, was based on the Cleric class. The second and third episodes, which remained unreleased, were intended to be based on the warrior and sorceress class respectively. The team had chosen to use an episodic approach in order to aim for a manageable release schedule.

This was the grand vision for HEOC. It seems like the project is dead, but they released this demo in 2010 which can now be played with dhewm3.

After a long time...

**A monumental thank you to Zeroth for releasing this mod under GPL and to _LegendGuard_ for organizing this and doing the initial dhewm3 port!**

## How to build

### On Windows

You need [CMake](https://cmake.org/) either Visual Studio (2010 and newer have been tested) or [MinGW-w64](https://mingw-w64.org/)

1. Clone the dhewm3-sdk git repo
2. switch to your git clone's directory: `cd dhewm3-sdk`
2. (optional: switch to an existing mods branch: `git checkout eoc`)
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
3. (optional: switch to an existing mods branch: `git checkout eoc`)
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

## License

The code changes of HEOC have been released under GPLv2 or later (by Brandon Captain (AKA Zeroth)). As the Doom 3 source code (that the dhewm3 SDK is based on) was released under GPLv3 or later, that's the effective license of this code.

## Credits

### The Team

**Andreas Håndlykken (AKA Slash)**  
Lead Concept Artist, Texture Artist, Modeler, Pirate Captain

**Bas de Zwart (AKA BloodRayne)**  
Project Founder. Models, Textures, Weapons, Scripting

**Brandon Captain (AKA Zeroth)**  
SDK Programmer, Scripting, SVN/Server host and Admin

**Caffeine Freak**  
Lead Level Designer. All of HUB 1 - H1M1(detailing/scripting), H1M2(All), H1M3(All)

**Calavera_Jo**  
Coder

**Deadite4**  
Sound FX/VO Engineer, Handyman, Coordinator, PR

**Justin Lassen**  
Composer, All Music

**Mystic Realmz**  
Composer

**Parsonsbear**  
Pickup and Map Models/Textures

**Shadow**  
Level Designer

**Solarsplace**  
Coder

**Spookt**  
Writer. Story Arc, Journals

**TomJ**  
Level Designer, Hub 2 (H2M1(future release))

<br/>

### Ocassional Contributors

**BHenderson**  
Animations

<br/>

### Past Members

**CodenniumRed**  
Level Designer, H1M1 Layout

**Gazz**  
Level Designer, H1M3(1st initial block out)

**Gman**  
Static Models

**RazorBladder**  
Lead Texture Artist. Models, Textures

**Ruckus**  
Map Models

**Slyrr**  
Lead Creature Models, Lead Animations

**Zurra**  
Models, Textures

<br/>

### Testers

**Indoril Lichenvale**  
**JC316**  
**RaVeN**  
**Redeemer812**

<br/>

### Special Thanks

**LegendGuard**  
Who ported to dhewm3
