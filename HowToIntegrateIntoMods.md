# How to integrate Doom3[CC] into Doom3 or dhewm3 SDK C++ code

.. as of 2020-01-27, using the source code checked into https://github.com/dhewm/dhewm3-sdk/tree/doom3cc

First, copy the `d3cclib/` directory into your mods source directory and add
`d3cclib/CCBst.cpp` to your build (VS project, scons, CMakeLists.txt, depending on
howr you build your mod).  
For the dhewm3 SDK, you just add it to the `src_game_mod` or `src_d3xp_mod` source lists
at the beginning of `CMakeLists.txt`.

Now you need to integrate Doom3[CC], or more specifically the `ccBst` class into
the mods sourcecode, which thankfully is pretty easy (all file names/paths are relative
to the game/ or d3xp/ directory, depending on where your code is).

### In **Game_local.h**:

First you make sure the `ccBst` class and its methods are available to your other source files
and that an instance of that class is held somewhere - in the `idGameLocal` class:

1. After `#include "MultiplayerGame.h"`, add a line with  
   `#include "../d3cclib/CCBst.h"`
2. In the `idGameLocal` class, after the `int						lastGUI;` line, add:  
   `ccBst					ccDisplayInfo;`

### In **Game_local.cpp**:

Here you'll make sure `ccBst` is initialized and updated so it can do its thing:

1. In `void idGameLocal::Init( void )` between `smokeParticles = new idSmokeParticles;`
   and `// set up the aas` add:  
   `ccDisplayInfo.InitOnce();`
2. In `void idGameLocal::LoadMap( const char *mapName, int randseed )`, before
   `// clear the smoke particle free list` add:
   ```c++
   	// will only initialize the colors once
   	if(!ccDisplayInfo.GetColorInit())
   	{
   		ccDisplayInfo.Init("/caption/colors.dcc", false);
   	}
   	ccDisplayInfo.Init(mapFileName, false);
   ```
3. In `gameReturn_t idGameLocal::RunFrame( const usercmd_t *clientCmds )`, between
   `FreePlayerPVS();` and `// do multiplayer related stuff`, add:  
   `ccDisplayInfo.Update();`

### In **Player.cpp**:

Right at the beginning of `void idPlayer::DrawHUD( idUserInterface *_hud )`, comment
out the first if-block, like:

```c++
	/* commented out for d3cc
	if ( !weapon.GetEntity() || influenceActive != INFLUENCE_NONE || privateCameraView || gameLocal.GetCamera() || !_hud || !g_showHud.GetBool() ) {
		return;
	}
	*/
```

The subtitles are rendered as part of the HUD and this makes sure the HUD is always drawn
so the subtitles are also shown in cutscenes etc.

### In **Entity.cpp**:

In `bool idEntity::StartSoundShader( const idSoundShader *shader, ...)`, after  
```
	if ( length ) {
		*length = len;
	}
```
and before `// set reference to the sound for shader synced effects`, add:  
```c++
	idVec3 temp;
	idMat3 temp2;
	if ( !GetMasterPosition(temp, temp2) )
	{
		temp = GetWorldCoordinates(physics->GetOrigin());
	}
	gameLocal.ccDisplayInfo.Display( idStr(shader->GetSound(0)), len, temp, shader->GetMaxDistance(), name );
```

This tells the subtitle system to display a subtitle for the currently played sound (if one exists)
and also sets the position of the sound in the world, so it can be shown on the radar
and it can be decided if the subtitle should be shown at all (or if it's too far away from the player).


### That's all!

Ok, for the code at least.  
You'll still have to write the subtitles and adjust the GUIs etc, but that's currently not documented here.
Have a look at Doom3[CC]'s game data on how that's done.
