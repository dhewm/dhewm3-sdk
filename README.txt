NOTE from Daniel: 
The following is (mostly) the original README of the mod and may refer to stuff that 
is not contained in this sourcecode like shaders and other game data.
You can download the orignal mod (including the game data) from:
  http://doom3.filefront.com/file/Dentons_Enhanced_Doom3;76838 
(Or search for dentons_enhanced_doom3_2.02.zip)

Download it, unzip it and unzip the contained zip to your Doom3/dhewm3 installation
BUT MAKE SURE to rename the Dentonmod/ dir to dentonmod/, as doom3 doesn't like
uppercase characters in paths (while it works on Windows, it doesn't on Linux, 
FreeBSD and possibly OSX!)

After compiling the source, you can start the game with
  ./dhewm3 +set fs_game dentonmod
  
A big THANK YOU to "Clone JC Denton" for releasing his mod under GPL!

The original ReadMe:
==================================================================================================
------------------------------ Denton's Enhanced Doom3 --------------------------------------------
==================================================================================================
By - Clone JC Denton 				(AKA Vicky Jadhav)
Contact- clone_jc_denton AT yahoo DOT com
==============================================
     

THANK YOU!

Thank you for downloading this mod.


======================================================================================
OVERVIEW
======================================================================================
This mod is version 2.02 of Denton's SDK based enhancement (Rated 9.7 out of 10 by filefront community). I received a very nice feedback as well as some constructive criticism from the people who played the previous version and therefore, in addition to many new features, this version tries to fix some issues that were addressed by the people. 

Some of the new features are:
- New ultra quality yet highly scalable(DX9 only) bloom shader with HDR like shift sensitivity. There are now three types of bloom.  
- This mod enables adding any number of particle effects and lights to weapons in a really easy way. New cool particle effects for muzzleflashes and weapon mods such as flashlight and laser sight etc. 
- Material based decals, particles and sound effects.
- Denton's Ultra real stereo surround sound pack that takes advantage of new unified way of playing impact sounds.
- New customizable tracer effects, all new Plama-railgun with cool railbeam effect. 

These features and lots of other improvements, I am really sure, will take your Doom 3 experience to a whole new level. This mod may also prove useful to mod makers as well as SP-map makers. 

Actually, there were more of those whole new things in the development pipeline. But I have been busy with all sort of things but modding and the progress on this mod was halted for more than 3 months, so I am releasing it as it now. 

================================================================================================
DLL related Changes
================================================================================================

# New Bloomshader: The new bloom uses heavily modified use_less' bloomshader as base and is for DX9 compliant hardware only. There are three types of bloom now. All these three types use 2 pass Gaussian blur and the first two types support HDR like shift sensitivity (taken from maha_x's bloomshader). The shifting of sensitivity is not as perfect as HDR, it works much like oblivion's HDR based shift sensitivity effect. The new bloomshader not only is works better on low end systems (with DX9 hardware) but also can provide ultra high quality bloom for high end systems.

# New weaponFX system: That let add any number of particle effects and lights to weapons in a really easy way. All you need to do is add some keys to weapon defs and use some simple scripts. 

# RealTracers 2.0 (tm): All new tracer logic for creating tracers and beams with customizable colors, size and speed etc. The tracer logic supports models (.lwo, .prt) as well as new customizable beam type. 

# Unified how decals, particle effects and sound applied to entity and world upon projectile impact.

# Added ability to attach smokeParticles to any entity: Ever wondered why all the hanging mummies, lying dead bodies or lying body parts(like dismembered hand, torso etc) never had blood spurting wound when shot at? Now, a smoke particle can be attached to any type of entity.

# A weapon now can have more than one projectile type but consumes the same ammo type: E.g. plasmagun now has two different projectile types: plasma-balls, and plasma-rail beam. In the similar manner you can have different projectile type for rocket launcher, such as normal rockets and grenades etc. Each of the different projectile type can have different recoil for the same weapon.

Some other changes include:
- New blood splat and blood pool decals system
- g_debugdamage when set to 1, now dumps collision material information to console.
- Improved behavior of exploding and burning barrels. 
- Improved the eject brass behavior and added more control over it.
- Added alternate fire key for weapons.
- Lots of bug fixes, improvements and optimizations.

All the features/changes above are discussed in detail in info.txt; where as previous version features can be found in readme_old.txt.

Previous version features summary (in short):

- Support for up to 32 weapons with shared ammo considerations.
- Half-Life style quick weapon key. 
- Improved Gibbing system.
- Tweaked the way ragdolls react to explosions, now an explosion can throw animated bodies in air.
- Physics based collision sounds for debris.

========================================================================================
Other changes -
========================================================================================

 #Includes Denton's Ultra Real stereo surround sounds v3.2 (unreleased) introducing new audio experience.
------------------------------------------------------------------------------------------------
Version 3.2 changes:
- Impact sounds for different materials like, metal, wood, glass, plastic, sand, stone and concrete etc.
- New Super shotgun reload sounds, so that the gun feels more like Doom 2 Super shotgun. Note that, these sounds are not directly taken from Doom 2.
- Readjusted loudness of many sounds so that weapons do not feel overpowered.
- New plasma-rail and plasma recharge sounds.
- New stereo-surround plasma-hum sound.
- Version 2 shotgun fire sound is now back. It feels more in place now. 
- Bullet_flight sound is now looping, this means you can hear whining sounds as tracers miss you, regardless of how long the monster that was shooting at you, is.  

(Previous version changes details can be found in readme_old.txt)
------------------------------------------------------------------------------------------------

# New Particle Effects:
- All new third person as well as first particle effects for muzzleflashes of weapons like, pistol, machinegun, shotgun, and chaingun. Initially, I created one or two particle effects which were more realistic, but they didn't fit nicely in Doom 3 universe and then I created muzzleflash effects that wont take away the original feel of weapons. 
- New smooth and natural looking muzzlesmoke effects that don't block you field of vision.
- New realistic particle effects for material based damage feedback, which include a unique effect for materials like metal, wood, concrete, sand, stone, plastic and cardboard.
- New realistic particle effects for rocket trail, rocket explosion, grenade explosion, barrel explosion, etc.

# added surface-type tags  to lots of materials to support new material based damage feedback. Some materials were left unmodified though, since it was really a tiring job.  

# Added Super Shotgun with it's GUI icon and PDA info. Now, you can actually pick up the super shotgun in admin level and later in Delta5 lab when you come back from hell.

# Weapon tweaks:
- Pistol rate of fire is increased so that it fires almost as fast as you shoot, but the base accuracy is affected depending on how fast you fire it.
- Added lasersight to pistol, which can be turned on/off using alt fire key (Mouse2 by default)
- Machinegun fire with normal rate of fire and has lesser spread, but the spread increases as you hold the trigger. So, for better accuracy, you need to fire in bursts.
- added flashlight attachment to shotgun which can now be turned on/off using alt fire key (Mouse2 by default)
- Added alternate fire to plasmagun. The secondary fire mode now fires plasmarail which consumes 10 plasma balls and does damage which is slightly less than damage done by 10 plasma balls. 

# Wrote many new and modified existing skins, materials, defs. Added some new decal textures. Added new textures for different particle effects. 

# GUI related changes:
- You can now configure alternate attack key by going to GUI window, main menu -> options -> control -> attack/look.
- Fixed an issue where multiplayer weapon icons were not showing correctly due to the addition of super shotgun.

# Includes ultra perfromance tweak(see autoexec.cfg and doomConfig.cfg) by Compguru76 which removes stuttering altogether at high and ultra high quality. I have modified this tweak to solve some compatibility issues with some ATI graphics cards.

# Previous version changes summary in short:
(refer to readme_old.txt for details)

- improved articulated figures (ragdolls) for all monsters.
- Fixed mass and density of many physics objects so that they react realistically to explosions and when shot by weapons.
- Locations based damages for all monsters except bosses.
- Tweaked projectile physics values for better kickback upon impact.
- Added Super shotgun with interruptible reload and appropriate eject brass behavior.
- New chaingun spinup behavior, now feels more realistic
- New particle effects for various explosions, impacts, projectile trails, monster burnaway etc

================================================================================================
Version 2.02 change summary (since version 2.0) 
================================================================================================
- Added support for Doom 3 v1.3.1 as well as v1.3.0 (DLL related change)
- Fixed bug where shotgun reloaded 8 shells in one go (DLL related change, see info.txt for details)
- Fixed bug where plasma-rail consumed 19 projectiles where as it should consume only 10.
- Fixed a bug where william banks' pda showed as default pda in admin lab. 
- Readjusted loudness super-shotgun reload sound.
- Miscellaneous small fixes.

================================================================================================
 MAKING A SP MAP COMPATIBLE WITH THIS MOD 
================================================================================================
Basically, any SP map should work well with this mod. Only the thing is, if the map is using custom materials, a default smoke and spark effect will be used for all those. If you like this mod and you are making any SP maps that you might want to make compatible with material based damage feedback system, remember to add tags (like wood, stone, plastic etc. ) to your custom materials.

Refer to info.txt for some more details. 

================================================================================================
 FEEDBACK
================================================================================================
If you have any queries regarding to this mod feel free to mail me. There are no known issues with this mod. However, if you find any, please let me know. 

================================================================================================
 CREDITS
================================================================================================

Scripting, decl modifications, GUI Scripting, Particle Effects, Shader Programs(for bloomshader), DLL Coding (VC++) all done by Vikram Jadhav (AKA Clone JC Denton)

Original Bloomshader1 code (with Gaussian Blur)		- Use_less and SantaClaws
Original Bloomshader2 code (with shift sensitivity)	- maha_x (maha_x[at]hotmail[dot]com)

Following features/bug fixes were contributed to this mod by Konrad Kulczycki (tadeusz[dot]kulczycki[at]neostrada[dot]pl):
------------------------------------------------
Placed super-shotgun in admin map and delta5 map.
Fixed bug where weapons were taken off of or added to the player inventory in a wrong way when teleporting to or from hell respectively.
Added pda icon and description for super-shotgun.

He had also added gravitygun and placed in maps like, mc_underground, enpro and delta5, but I've not included it in this release.
------------------------------------------------

Ultraperformance tweak - By compguru76 (compguru76 AT yahoo DOT com)

I would like to thank use_less and maha_x not only for writing such a wonderful bloomshaders but also allowing me to use their code in my mod. I would also like to thank oneofthe8devilz for providing me use_less' code before I could get in touch with use_less. 

As per the original plans, a heavily modified gravity gun (originally created by bloodrayne) was going to be part of this mod. I am thankful to Bloodrayne for allowing me to include his work, but I couldn't give time to finish my work with the gravgun. 

I'd also like to thank my friend, Konrad Kulczycki for his contribution to my mod and for his patience. Actually, he had contributed his work almost a year back, when I was planning a minor update for the first version.  

Last but not least, I'd like to thank all the people who downloaded the previous version and showered me with really excellent feedback. 

================================================================================================
 What's missing? 
================================================================================================

Following are the features I had originally planned to add to this mod. Due to the limited time I had, I couldn't develop them. I may complete some of them and release thro' an update/patch. However, I can't guarantee anything now. 

Note that, these features are not exaggerations of any sort, they can be hundred percent implemented.

# Advanced weaponFX system that works for almost every entity. Which can be used for:
 - custom muzzleflashes and weapon mods (e.g. laser, flashlight etc) that works on monsters' weapons,
 - Setting up a grenade as laser trip mine, etc.

# Gravity gun with half-life 2 like functionality and with DLL based support. It was placed in some of the maps (See credits section), so that you can actually pick it up instead of using cheat codes. 

# BlurVision effect when player receives melee attack from monsters.

# Weapon zooming system with visible scope GUI.

# Simulated particle bouncing for sparks and debris that comes of different materials when shot at.

# Advanced weapon-beam system that gives target information, which can be used for 
 - weapons with laser beams, where there accuracy is dicated by laser beams,
 - laser guided rockets,
 - target monitoring for gravity gun

================================================================================================
 DISCLAIMER
================================================================================================
- This mod is tested to work with single player only. The new tracer effects and muzzleflash effects work for multiplayer too, but none of them have tested thoroughly. 
- This mod is for Doom 3 only and does not work for Doom 3 ROE.
- This mod does not work with any other mod that has their own gamex86.dll. 

You can use any of the mod assets and dll as you see fit, but don't forget to credit me. I'll be glad if you'd mailed me to notify that you are using it. Also, I am not releasing the code as of now, which I plan to release soon. Though, I'll mail the code on request. 

===============================================================================================
