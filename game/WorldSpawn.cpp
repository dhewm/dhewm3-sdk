/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 GPL Source Code ("Doom 3 Source Code").

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include "sys/platform.h"
#include "framework/FileSystem.h"

#include "gamesys/SysCvar.h"
#include "script/Script_Thread.h"

#include "WorldSpawn.h"

/*
================
idWorldspawn

Worldspawn class.  Each map has one worldspawn which handles global spawnargs.
Every map should have exactly one worldspawn.
================
*/
CLASS_DECLARATION( idEntity, idWorldspawn )
	EVENT( EV_Remove,				idWorldspawn::Event_Remove )
	EVENT( EV_SafeRemove,			idWorldspawn::Event_Remove )
END_CLASS

/*
================
idWorldspawn::Spawn
================
*/
void idWorldspawn::Spawn( void ) {
	idStr				scriptname;
	idThread			*thread;
	const function_t	*func;
	//const idKeyValue	*kv;		<-- not used in grimm

	// grimm -->
	
	gameSoundWorld->SetSlowmo( false );
	gameSoundWorld->SetSlowmoSpeed( 0.8f );

	// <-- grimm

	assert( gameLocal.world == NULL );
	gameLocal.world = this;

	g_gravity.SetFloat( spawnArgs.GetFloat( "gravity", va( "%f", DEFAULT_GRAVITY ) ) );

	// disable stamina on hell levels
	if ( spawnArgs.GetBool( "no_stamina" ) ) {
		pm_stamina.SetFloat( 0.0f );
	}

	// load script
	scriptname = gameLocal.GetMapName();
	scriptname.SetFileExtension( ".script" );
	if ( fileSystem->ReadFile( scriptname, NULL, NULL ) > 0 ) {
		gameLocal.program.CompileFile( scriptname );

		// call the main function by default
		func = gameLocal.program.FindFunction( "main" );
		if ( func != NULL ) {
			thread = new idThread( func );
			thread->DelayedStart( 0 );
		}
	}

	// grimm --> set the current ambient light levels for this session
	SetAmbientLight();
	// <-- grimm

	/* grimm --> not needed for us. I call everything from main anyway.

	// call any functions specified in worldspawn
	kv = spawnArgs.MatchPrefix( "call" );
	while( kv != NULL ) {
		func = gameLocal.program.FindFunction( kv->GetValue() );
		if ( func == NULL ) {
			gameLocal.Error( "Function '%s' not found in script for '%s' key on worldspawn", kv->GetValue().c_str(), kv->GetKey().c_str() );
		}

		thread = new idThread( func );
		thread->DelayedStart( 0 );
		kv = spawnArgs.MatchPrefix( "call", kv );
	}
	*/ 
	// <-- grimm
}

/*
=================
idWorldspawn::Save
=================
*/
void idWorldspawn::Save( idRestoreGame *savefile ) {
}

/*
=================
idWorldspawn::Restore
=================
*/
void idWorldspawn::Restore( idRestoreGame *savefile ) {
	assert( gameLocal.world == this );

	g_gravity.SetFloat( spawnArgs.GetFloat( "gravity", va( "%f", DEFAULT_GRAVITY ) ) );

	// disable stamina on hell levels
	if ( spawnArgs.GetBool( "no_stamina" ) ) {
		pm_stamina.SetFloat( 0.0f );
	}

	// grimm --> set the current ambient light levels for this session
	SetAmbientLight();
	// <-- grimm
}

/*
================
idWorldspawn::~idWorldspawn
================
*/
idWorldspawn::~idWorldspawn() {
	if ( gameLocal.world == this ) {
		gameLocal.world = NULL;
	}
}

/*
================
idWorldspawn::Event_Remove
================
*/
void idWorldspawn::Event_Remove( void ) {
	gameLocal.Error( "Cannot remove the world." );
}

// grimm --> sets the ambient type for the current session, specified in the worldspawn entity (any brush in the map)
/*
================
idWorldspawn::SetAmbientLight
================
*/
void idWorldspawn::SetAmbientLight( void ) {
	int AmbientType = spawnArgs.GetInt("AmbientType");
	gameLocal.Printf("Ambienttype set to %i.\n", AmbientType);

	if ( AmbientType == 0 ) {
			cvarSystem->SetCVarString( "g_ambientLightColor", "0.12 0.06 0.04" );		
	} else if ( AmbientType == 1 ) {
			cvarSystem->SetCVarString( "g_ambientLightColor", "0.04 0.07 0.11" );
	} else if ( AmbientType == 2 ) {
			cvarSystem->SetCVarString( "g_ambientLightColor", "0.08 0.08 0.08");
	} else if (AmbientType == 3) {
			cvarSystem->SetCVarString( "g_ambientLightColor", "0.04 0.12 0.06");
	} else {
			cvarSystem->SetCVarString( "g_ambientLightColor", "0.12 0.06 0.04" );
	}	
}
// <-- grimm
