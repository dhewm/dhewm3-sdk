# Fitz Packerton

This is the source of the "Fitz Packerton" game by Brendon Chung, Teddy Dief,
Ryan Cousins and Sarah Elmaleh, ported to the dhewm3 SDK.

The original source can be downloaded at http://blendogames.com/older.htm

Fitz Packerton is "A theatrical game about a man and the things he carries.
Pack what you must. Pray you won't need it."

You can get the game at https://teddydief.itch.io/fitz

Fitz Packerton is actually an (experimental, short) standalone game, so you
don't even need the Doom3 gamedata to run it, only the Fitz data linked above.

you could start it with
`./dhewm3 +set fs_basepath /path/to/fitzpackerton/ +set fs_game fitz`

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
8. it (e.g. `fitz.so`) should now be in the build/ directory,
   copy it to your dhewm3 install, where base.so and d3xp.so are
9. start the game with that mod, like `dhewm3 +set fs_game fitz`

(Make sure to actually have the mods game data in the right directory as well;
the directory name should be the same as the game lib name, but without .so/.dylib/.dll,
for example dentonmod/ for dentonmod.so)

On Windows it should be kinda similar, but you have to tell CMake to create
a Visual Studio solution for the VS version you're using, or Makefiles for
MinGW-w64 or whatever.
