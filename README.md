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
[Blood Mod](https://github.com/dhewm/dhewm3-sdk/tree/bloodmod),
[Classic Doom 3](https://github.com/dhewm/dhewm3-sdk/tree/cdoom),
[Denton's Enhanced Doom3](https://github.com/dhewm/dhewm3-sdk/tree/dentonmod),
[Fitz Packerton](https://github.com/dhewm/dhewm3-sdk/tree/fitz),
[Hard Corps](https://github.com/dhewm/dhewm3-sdk/tree/hardcorps),
[Perfected Doom 3](https://github.com/dhewm/dhewm3-sdk/tree/perfected),
[Scarlet **Rivensin**: The Ruiner](https://github.com/dhewm/dhewm3-sdk/tree/rivensin),
[Doom3: The Lost Mission](https://github.com/dhewm/dhewm3-sdk/tree/d3le) and
[Sikkmod](https://github.com/dhewm/dhewm3-sdk/tree/sikkmod)

In addition to this repository, there is also the LibreCoop mod that implements Coop
gameplay for dhewm3: [LibreCoop Github for the source code](https://github.com/Stradex/librecoop)
and [LibreCoop on ModDB which has the game data](https://www.moddb.com/mods/librecoop-dhewm3-coop).

You can find Win32 DLLs and Linux x86_64 (amd64) `.so` libraries of those mods that work with
dhewm3 1.5.x at [the dhewm3 Github release page](https://github.com/dhewm/dhewm3/releases/latest)
(the dhewm3-mods-1.5.* archives).

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

Please note that currently I only accept mods that are released under the
GPL license - the one used by Open Source Doom3 (i.e. *not* only the Doom3 SDK license) -
because neither the GPL nor the SDK license allow merging code from both licenses.  
So please get permission from the mod authors first.

The usual (easiest) way to port a mod is to make a diff between the mod's source
and the Doom3 SDK and apply the resulting patch to the vanilla game source (from the master branch).  
Afterwards usually some manual work must be done to resolve patching conflicts and get the mod to compile.  
Also, the CMakeLists.txt file must be adjusted (see the dentonmod branch for examples).

Here is the approximate steps I use to port a mod.  
Note that you'll need basic C++ (or at least C) programming skills, so you can resolve the little
(and sometimes not-so-little) issues that (almost) always occur when porting a mod, like merge
conflicts and compiler errors due to missing `#include`s.

#### Getting the difference between the original Doom3 SDK and the Mod's source code

IMHO, the easiest way is to use a git repo of the the origin Doom3 SDK source code, copy the modified
source from the mod on top and then let git create the diff.

For your convenience, I created such a git repo: https://github.com/DanielGibson/Doom3-SDK

So clone it and switch into its directory, by running the following commands in a terminal
(on Windows, use the "Git Bash"):
* `cd doom3dev/` (change into a directory you want to put Doom3 projects in, adjust this to your needs)
* `git clone https://github.com/DanielGibson/Doom3-SDK.git`
* `cd Doom3-SDK`

Now copy the mod's source code to the correct place in the repo, usually `src/` or `src/game`,
replacing the existing files.  
`git status`  
shows which files have been changed,  
`git diff`
will show the actual differences *(but only for files that already exist in the repo!)*; or use  
`git gui`
for a GUI-based overview.

It's possible that the copied files use different line endings than the git repo, in which case git
will show lots of changes that are really none.

You can fix the line endings by running the following commands:  
* `find -iname "*.cpp" -exec dos2unix {} \;`  
* `find -iname "*.h" -exec dos2unix {} \;`  
    - *If you're on **Windows**, use `unix2dos` instead of `dos2unix`*.

Now **create a branch** for the mod in your local Doom3-SDK repo and stage all the changes,
including added files:  
* `git checkout -b mymodname` (adjust the name...)  
* `git add --all`  

I recommend using `git gui` to check if any files have been added that are irrelevant, i.e. files
that are not source files but from build directories or in Visual Studio project files or similar
(those won't be needed for dhewm3-sdk, it uses CMake to handle the build).  
You can *unstage* files or changes by clicking the file icon left of the filename
in the "Staged Changes" list to remove them from the commit (the files/changes are *not* deleted,
they will just not be committed).

> If you insist on using the commandline,  
> `git status`  
> will also show the staged files, and you can unstage files with  
> `git rm --cached path/to/file.name`  
> or unstage whole directories with  
> `git rm --cached -r path/to/`

Now commit the changes, either on the commandline with  
`git commit -m "imported mymodname"`  
or by just typing a commit message in `git gui` and clicking "Commit" there.

Finally, to get a diff patch you can apply to the dhewm3 SDK, run the following command:  
`git diff main > ../mymodname.diff`

#### Applying the mod diff to the dhewm3 SDK

First, clone the dhewm3 SDK and create a branch for the new mod, with the following terminal commands
(again, on Windows, use the "Git Bash"):  
* `cd doom3dev/` (same directory as used in the other step)
* `git clone https://github.com/dhewm/dhewm3-sdk.git`  
* `cd dhewm3-sdk`  
* `git checkout -b mymodname`

> If you've already cloned the repo earlier, make sure to check out the `master` branch before
> creating the new branch for your mod, so the new branch is based on the unmodified gamecode,
> and make sure there are no uncommitted changes:  
> * `git checkout master`
> * `git reset --hard HEAD` (undo all uncommitted changes)
> * `git pull` (get the latest changes from the dhewm3-sdk repo)

Now apply the patch with the mod's code with:

`patch -p2 -l --merge --no-backup-if-mismatch < ../mymodname.diff`

Explanation:
* `-p2` skips an additional directory layer: In the Doom3 SDK the source `src/game/`, for example,
  in the dhewm3 SDK it's directly in `game/`, so `src/` must be skipped
* `-l` ignore whitespace changes, in case the mod has replaced tabs with spaces or something
* `--merge` when there are merge conflicts (patch isn't sure how to apply a change to the dhewm3 SDK),
  they are marked in the corresponding source files with sections containing the new code and the old
  code, marked with `<<<<<<<`, `=======` and `>>>>>>>` (see below)
* `--no-backup-if-mismatch` if this is not set, patch will create `bla.cpp.orig` (containing the
  original unpatched file) for every file that's patched, we don't want that

Now look very carefully at the output the `patch` command printed to the terminal!  
It often happens that some changes can't be merged automatically, and `patch will tell you about that`
like this:

```
patching file game/Misc.cpp
Hunk #1 NOT MERGED at 143-148.
```

This means that one change in game/Misc.cpp could not be merged automatically, so it must be merged manually.  
`patch` then inserts something like this in that file:

  ```
   ...
   switch( event ) {
      case EVENT_TELEPORTPLAYER: {
         entityNumber = msg.ReadBits( GENTITYNUM_BITS );
         idPlayer *player = static_cast<idPlayer *>( gameLocal.entities[entityNumber] );
         if ( player != NULL && player->IsType( idPlayer::Type ) ) {
            Event_TeleportPlayer( player );
         }
         return true;
      }
      default:
         break;
      }
  <<<<<<<
  
  	return idEntity::ClientReceiveEvent( event, time, msg );
  =======
  	//	return false;	// sikk - warning C4702: unreachable code
  >>>>>>>
  }
  ```

So open the file and search for "<<<<".
The first section (between `<<<<<<<` and `=======`) is the existing code of dhewm3-sdk, the second
section (between `=======` and `>>>>>>>`) is what that code looked like in the mod you're trying to
merge (in this example Sikkmod).  
  
> In this case, both Sikkmod and dhewm3 fixed a compiler warning: 
> `return idEntity::ClientReceiveEvent( event, time, msg );` used to be in the `default:` case of switch,
> and after the switch was `return false;`, which was unreachable because the function would always
> return at `default: return idEntity::ClientReceiveEvent( event, time, msg );` (if it didn't already 
> return before), and the compiler warned about that unreachable code.  
> *sikk* got rid of that warning by commenting out `return false;`, in dhewm3 we moved the
> `return idEntity::ClientReceiveEvent( event, time, msg );` behind the switch-case - both valid 
> (and equivalent) solutions.

You need to remove the lines with `<<<<<<<` and `=======` and `>>>>>>>`, and make sure that the code that
was in this sections is merged completely, i.e. in a state that works correctly like it did in the mod.

> In this example it's simple: Just keep dhewm3's code and remove sikk's change:
>  ```
>  ...
>      switch( event ) {
>         case EVENT_TELEPORTPLAYER: {
>            entityNumber = msg.ReadBits( GENTITYNUM_BITS );
>            idPlayer *player = static_cast<idPlayer *>( gameLocal.entities[entityNumber] );
>            if ( player != NULL && player->IsType( idPlayer::Type ) ) {
>               Event_TeleportPlayer( player );
>            }
>            return true;
>         }
>         default:
>            break;
>      }
>     
>      return idEntity::ClientReceiveEvent( event, time, msg );
>  }
>  ```

Sometimes it makes sense to open the file from Doom3 SDK and the patched one from dhewm3 SDK
side-by-side to compare functions with merge conflicts to see more context from the original file.

If you're *really* unlucky, `patch` will show you messages like
```
patching file d3xp/Item.cpp
Hunk #1 merged at 653.
misordered hunks! output would be garbled
Hunk #2 FAILED at 77.
misordered hunks! output would be garbled
Hunk #3 FAILED at 109.
2 out of 4 hunks FAILED -- saving rejects to file d3xp/Item.cpp.rej

```
This means that it has no idea whatsoever where that code from the patch belongs, and it will *not*
create such a merge-conflict section in the file as shown above. In that case you'll have to check
the `.rej` files for what changes have been omitted and try to merge them manually.
In this case, even the first hunk (which was supported to be around line 50) was, for reasons unclear
to me, merged at the totally wrong location so it must be fixed as well.

When merging the changes for a file fails completely, it can help to use a graphical diff and merge
tool like [meld](https://meldmerge.org/) or [kdiff3](https://kdiff3.sourceforge.net/) or 
[Beyond Compare](https://www.scootersoftware.com/) to compare the file from the Doom3 SDK and the one
from the dhewm3 SDK and merge the changes in there.  
However note that it will also show you differences that are unrelated to the mod, like fixes made
in dhewm3 - and the first lines of the file are always different, because in the dhewm3 SDK they
contain the [GPL license note from the Doom3 GPL release](https://github.com/dhewm/dhewm3-sdk/blob/master/d3xp/Item.cpp#L1-L27),
while in the original Doom3 SDK there usually is only a very short comment like
```
// Copyright (C) 2004 Id Software, Inc.
//
```
This is also the reason why it's easiest to create a diff in the Doom3 SDK and apply that diff
in the dhewm3 SDK (instead of using a merge tool on all files, for example): The diff only contains
the changes made in the SDK, so those copyright notices are not in the diff (unless the Mod author
changed those lines), and don't create merge conflicts in dhewm3 code.

In my experience, most of the changes from the patch are merged without any conflict, and then you'll
have a handful of `Hunk #X NOT MERGED` errors that at least can be resolved within the file.  
I've only ever seen the `Hunk #X FAILED` error in one project..

<br>
Anyway, when you think you've resolved all merge conflicts, you can make double-sure like this:  

`grep -r "<<<<" `  
and  
`grep -r ">>>>"`

both shouldn't find anything, at least not in .cpp or .h files.

Once all merge conflicts are resolved, remove any `.rej` files and commit the changes, like described
above (or just use `git gui` for that).  
Don't forget to also commit added source files, if any - in fact, remember which (source) files were
added, because they're needed in the next step!

#### Build the Mod for dhewm3

Now edit `CMakeLists.txt`.  

If the mod only uses the code in `game/`, you can set the `ON` in `option(D3XP  "Build the d3xp/ game code" ON)`
to `OFF`, if it only uses the code in `d3xp/`, you can do the same for `option(BASE ...` (if both are used,
i.e. the mod builds two DLLs, one for the base game and one for Resurrection of Evil, leave those options as they are).  

Make sure to adjust `BASE_NAME` and/or `D3XP_NAME` according to the mod directory name, for example,
the *Classic Doom 3* mod directory is called `cdoom` and it uses the source code in `game/`, so
the line is adjusted like `set(BASE_NAME   "cdoom" CACHE STRING "Name of the mod...")`.  

If the mod requires definitions passed to the compiler (like `-DMY_OPTION` for `#ifdef MY_OPTION`),
adjust `BASE_DEFS` and/or `D3XP_DEFS` accordingly.

Last but not least, if the mod adds any source files to the SDK (instead of just modifying the
existing ones), add them to `src_game_mod` or `src_d3xp_mod`.

Look at the
[CMakeLists.txt of the Rivensen Mod](https://github.com/dhewm/dhewm3-sdk/blob/rivensin/CMakeLists.txt)
for an example that does several of the things mentioned above (disable D3XP DLL, set custom compiler
definitions, add custom source files).

Once that's done, you can finally try to build the mod, as described in the [How to build section](#how-to-build).

You'll likely get compiler errors because of missing includes, or maybe there's
`#include "../idlib/precompiled.h"` or similar somewhere which is an error because dhewm3 doesn't
have `precompiled.h` (so remove that). It's usually best to scroll up to the first compiler error
and fix it (for example, if it complains that unknown type is used, that type is likely defined in
a header that must be included) and retry building, because often further errors are caused by the
first one, and fixing it fixes several others as well, so by building again after fixing the first
you'll see which errors remain.

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

### The Script Debugger

dhewm3 1.5.2 and newer contain the Script Debugger (that you may remember from Quake4).  
While most of the code for it is in the engine, the game code also needs some small changes for it to work.

The [debugger branch](https://github.com/dhewm/dhewm3-sdk/tree/debugger)
has the necessary changes in its last commit.

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
