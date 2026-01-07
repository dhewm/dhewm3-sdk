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
#include "script/Script_Thread.h"
#include "Player.h"
#include "ai/AI.h"

#include "Trigger.h"

/*
===============================================================================

  idTrigger

===============================================================================
*/

const idEventDef EV_Enable( "enable", NULL );
const idEventDef EV_Disable( "disable", NULL );

CLASS_DECLARATION( idEntity, idTrigger )
	EVENT( EV_Enable,	idTrigger::Event_Enable )
	EVENT( EV_Disable,	idTrigger::Event_Disable )
END_CLASS

/*
================
idTrigger::DrawDebugInfo
================
*/
void idTrigger::DrawDebugInfo( void ) {
	idMat3		axis = gameLocal.GetLocalPlayer()->viewAngles.ToMat3();
	idVec3		up = axis[ 2 ] * 5.0f;
	idBounds	viewTextBounds( gameLocal.GetLocalPlayer()->GetPhysics()->GetOrigin() );
	idBounds	viewBounds( gameLocal.GetLocalPlayer()->GetPhysics()->GetOrigin() );
	idBounds	box( idVec3( -4.0f, -4.0f, -4.0f ), idVec3( 4.0f, 4.0f, 4.0f ) );
	idEntity	*ent;
	idEntity	*target;
	int			i;
	bool		show;
	const function_t *func;

	viewTextBounds.ExpandSelf( 128.0f );
	viewBounds.ExpandSelf( 512.0f );
	for( ent = gameLocal.spawnedEntities.Next(); ent != NULL; ent = ent->spawnNode.Next() ) {
		if ( ent->GetPhysics()->GetContents() & ( CONTENTS_TRIGGER | CONTENTS_FLASHLIGHT_TRIGGER ) ) {
			show = viewBounds.IntersectsBounds( ent->GetPhysics()->GetAbsBounds() );
			if ( !show ) {
				for( i = 0; i < ent->targets.Num(); i++ ) {
					target = ent->targets[ i ].GetEntity();
					if ( target && viewBounds.IntersectsBounds( target->GetPhysics()->GetAbsBounds() ) ) {
						show = true;
						break;
					}
				}
			}

			if ( !show ) {
				continue;
			}

			gameRenderWorld->DebugBounds( colorOrange, ent->GetPhysics()->GetAbsBounds() );
			if ( viewTextBounds.IntersectsBounds( ent->GetPhysics()->GetAbsBounds() ) ) {
				gameRenderWorld->DrawText( ent->name.c_str(), ent->GetPhysics()->GetAbsBounds().GetCenter(), 0.1f, colorWhite, axis, 1 );
				gameRenderWorld->DrawText( ent->GetEntityDefName(), ent->GetPhysics()->GetAbsBounds().GetCenter() + up, 0.1f, colorWhite, axis, 1 );
				if ( ent->IsType( idTrigger::Type ) ) {
					func = static_cast<idTrigger *>( ent )->GetScriptFunction();
				} else {
					func = NULL;
				}

				if ( func ) {
					gameRenderWorld->DrawText( va( "call script '%s'", func->Name() ), ent->GetPhysics()->GetAbsBounds().GetCenter() - up, 0.1f, colorWhite, axis, 1 );
				}
			}

			for( i = 0; i < ent->targets.Num(); i++ ) {
				target = ent->targets[ i ].GetEntity();
				if ( target ) {
					gameRenderWorld->DebugArrow( colorYellow, ent->GetPhysics()->GetAbsBounds().GetCenter(), target->GetPhysics()->GetOrigin(), 10, 0 );
					gameRenderWorld->DebugBounds( colorGreen, box, target->GetPhysics()->GetOrigin() );
					if ( viewTextBounds.IntersectsBounds( target->GetPhysics()->GetAbsBounds() ) ) {
						gameRenderWorld->DrawText( target->name.c_str(), target->GetPhysics()->GetAbsBounds().GetCenter(), 0.1f, colorWhite, axis, 1 );
					}
				}
			}
		}
	}
}

/*
================
idTrigger::Enable
================
*/
void idTrigger::Enable( void ) {
	GetPhysics()->SetContents( CONTENTS_TRIGGER );
	GetPhysics()->EnableClip();
	gameLocal.SetPersistentTrigger( "trig", name.c_str(), true );
}

/*
================
idTrigger::Disable
================
*/
void idTrigger::Disable( void ) {
	// we may be relinked if we're bound to another object, so clear the contents as well
	GetPhysics()->SetContents( 0 );
	GetPhysics()->DisableClip();
	gameLocal.SetPersistentTrigger( "trig", name.c_str(), false );
}

/*
================
idTrigger::CallScript
================
*/
void idTrigger::CallScript( void ) const {
	idThread *thread;

	if ( scriptFunction ) {
		thread = new idThread( scriptFunction );
		thread->DelayedStart( 0 );
	}
}

/*
================
idTrigger::GetScriptFunction
================
*/
const function_t *idTrigger::GetScriptFunction( void ) const {
	return scriptFunction;
}

/*
================
idTrigger::Save
================
*/
void idTrigger::Save( idSaveGame *savefile ) const {
	if ( scriptFunction ) {
		savefile->WriteString( scriptFunction->Name() );
	} else {
		savefile->WriteString( "" );
	}
}

/*
================
idTrigger::Restore
================
*/
void idTrigger::Restore( idRestoreGame *savefile ) {
	idStr funcname;
	savefile->ReadString( funcname );
	if ( funcname.Length() ) {
		scriptFunction = gameLocal.program.FindFunction( funcname );
		if ( scriptFunction == NULL ) {
			gameLocal.Warning( "idTrigger_Multi '%s' at (%s) calls unknown function '%s'", name.c_str(), GetPhysics()->GetOrigin().ToString(0), funcname.c_str() );
		}
	} else {
		scriptFunction = NULL;
	}
}

/*
================
idTrigger::Event_Enable
================
*/
void idTrigger::Event_Enable( void ) {
	Enable();
}

/*
================
idTrigger::Event_Disable
================
*/
void idTrigger::Event_Disable( void ) {
	Disable();
}

/*
================
idTrigger::idTrigger
================
*/
idTrigger::idTrigger() {
	scriptFunction = NULL;
}

/*
================
idTrigger::Spawn
================
*/
void idTrigger::Spawn( void ) {
	GetPhysics()->SetContents( CONTENTS_TRIGGER );

	idStr funcname = spawnArgs.GetString( "call", "" );
	if ( funcname.Length() ) {
		scriptFunction = gameLocal.program.FindFunction( funcname );
		if ( scriptFunction == NULL ) {
			gameLocal.Warning( "trigger '%s' at (%s) calls unknown function '%s'", name.c_str(), GetPhysics()->GetOrigin().ToString(0), funcname.c_str() );
		}
	} else {
		scriptFunction = NULL;
	}
}


// HEXEN : Zeroth
void idTrigger::eoc_Check_Automap( idEntity *activator ) {
	idStr mapFloor = spawnArgs.GetString( "floor" );
	if ( mapFloor != "" && activator->IsType(idPlayer::Type)) {
		idPlayer *p1 =  static_cast< idPlayer * > ( activator );

		p1->AutoMapChange(mapFloor);
	}
}

/*
===============================================================================

  idTrigger_Multi

===============================================================================
*/

const idEventDef EV_TriggerAction( "<triggerAction>", "e" );

CLASS_DECLARATION( idTrigger, idTrigger_Multi )
	EVENT( EV_Touch,			idTrigger_Multi::Event_Touch )
	EVENT( EV_Activate,			idTrigger_Multi::Event_Trigger )
	EVENT( EV_TriggerAction,	idTrigger_Multi::Event_TriggerAction )
END_CLASS


/*
================
idTrigger_Multi::idTrigger_Multi
================
*/
idTrigger_Multi::idTrigger_Multi( void ) {
	wait = 0.0f;
	random = 0.0f;
	delay = 0.0f;
	random_delay = 0.0f;
	nextTriggerTime = 0;
	removeItem = 0;
	touchClient = false;
	touchOther = false;
	triggerFirst = false;
	triggerWithSelf = false;
}

/*
================
idTrigger_Multi::Save
================
*/
void idTrigger_Multi::Save( idSaveGame *savefile ) const {
	savefile->WriteFloat( wait );
	savefile->WriteFloat( random );
	savefile->WriteFloat( delay );
	savefile->WriteFloat( random_delay );
	savefile->WriteInt( nextTriggerTime );
	savefile->WriteString( requires );
	savefile->WriteInt( removeItem );
	savefile->WriteBool( touchClient );
	savefile->WriteBool( touchOther );
	savefile->WriteBool( triggerFirst );
	savefile->WriteBool( triggerWithSelf );
}

/*
================
idTrigger_Multi::Restore
================
*/
void idTrigger_Multi::Restore( idRestoreGame *savefile ) {
	savefile->ReadFloat( wait );
	savefile->ReadFloat( random );
	savefile->ReadFloat( delay );
	savefile->ReadFloat( random_delay );
	savefile->ReadInt( nextTriggerTime );
	savefile->ReadString( requires );
	savefile->ReadInt( removeItem );
	savefile->ReadBool( touchClient );
	savefile->ReadBool( touchOther );
	savefile->ReadBool( triggerFirst );
	savefile->ReadBool( triggerWithSelf );
}

/*
================
idTrigger_Multi::Spawn

"wait" : Seconds between triggerings, 0.5 default, -1 = one time only.
"call" : Script function to call when triggered
"random"	wait variance, default is 0
Variable sized repeatable trigger.  Must be targeted at one or more entities.
so, the basic time between firing is a random time between
(wait - random) and (wait + random)
================
*/
void idTrigger_Multi::Spawn( void ) {
	spawnArgs.GetFloat( "wait", "0.5", wait );
	spawnArgs.GetFloat( "random", "0", random );
	spawnArgs.GetFloat( "delay", "0", delay );
	spawnArgs.GetFloat( "random_delay", "0", random_delay );

	if ( random && ( random >= wait ) && ( wait >= 0 ) ) {
		random = wait - 1;
		gameLocal.Warning( "idTrigger_Multi '%s' at (%s) has random >= wait", name.c_str(), GetPhysics()->GetOrigin().ToString(0) );
	}

	if ( random_delay && ( random_delay >= delay ) && ( delay >= 0 ) ) {
		random_delay = delay - 1;
		gameLocal.Warning( "idTrigger_Multi '%s' at (%s) has random_delay >= delay", name.c_str(), GetPhysics()->GetOrigin().ToString(0) );
	}

	spawnArgs.GetString( "requires", "", requires );
	spawnArgs.GetInt( "removeItem", "0", removeItem );
	spawnArgs.GetBool( "triggerFirst", "0", triggerFirst );
	spawnArgs.GetBool( "triggerWithSelf", "0", triggerWithSelf );

	if ( spawnArgs.GetBool( "anyTouch" ) ) {
		touchClient = true;
		touchOther = true;
	} else if ( spawnArgs.GetBool( "noTouch" ) ) {
		touchClient = false;
		touchOther = false;
	} else if ( spawnArgs.GetBool( "noClient" ) ) {
		touchClient = false;
		touchOther = true;
	} else {
		touchClient = true;
		touchOther = false;
	}

	nextTriggerTime = 0;

	if ( spawnArgs.GetBool( "flashlight_trigger" ) ) {
		GetPhysics()->SetContents( CONTENTS_FLASHLIGHT_TRIGGER );
	} else {
		GetPhysics()->SetContents( CONTENTS_TRIGGER );
	}
}

/*
================
idTrigger_Multi::CheckFacing
================
*/
bool idTrigger_Multi::CheckFacing( idEntity *activator ) {
	if ( spawnArgs.GetBool( "facing" ) ) {
		if ( !activator->IsType( idPlayer::Type ) ) {
			return true;
		}
		idPlayer *player = static_cast< idPlayer* >( activator );
		float dot = player->viewAngles.ToForward() * GetPhysics()->GetAxis()[0];
		float angle = RAD2DEG( idMath::ACos( dot ) );
		if ( angle  > spawnArgs.GetFloat( "angleLimit", "30" ) ) {
			return false;
		}
	}
	return true;
}


/*
================
idTrigger_Multi::TriggerAction
================
*/
void idTrigger_Multi::TriggerAction( idEntity *activator ) {

	ActivateTargets( triggerWithSelf ? this : activator );
	CallScript();

// HEXEN : Zeroth
	eoc_Check_Automap( activator );
	//eoc_Check_Gravity();
/*
	if ( spawnArgs.GetBool("pauseGame") ) {
		gameLocal.paused = true;
	}
*/
	if ( spawnArgs.GetBool("killAllMonsters") ) {
		cmdSystem->BufferCommandText( CMD_EXEC_NOW, "massacre" );
		cmdSystem->BufferCommandText( CMD_EXEC_APPEND, "massacre" );
	}

	if ( spawnArgs.GetBool("stopAllSounds") ) {
		gameSoundWorld->StopAllSounds();
	}

	idPlayer *player = gameLocal.GetLocalPlayer();
	if ( player != NULL ) {
		// show custom gui, if any - MUST COME BEFORE PLAYER SPAWN ARGS STUFF BELOW
		idStr guiname;
		guiname = spawnArgs.GetString("showGui");
		if ( guiname != "" ) {
			player->OpenCustomGui( guiname );
		}

		// set player spawn args, if any
		const idKeyValue	*kv;
		idStr key;

		kv = spawnArgs.MatchPrefix( "setPlayerArg_", NULL );
		while( kv ) {
			key = kv->GetKey();
			key = key.Mid(13,key.Length());

			player->spawnArgs.Set( key.c_str(), kv->GetValue() );
			kv = spawnArgs.MatchPrefix( "setPlayerArg_", kv );
		}
	}
/*
	// HEXEN : Zeroth - set local player's variables to let the class know to save
	// some important info in his end-of-game save screen. that info will let us
	// be able to use save games from old releases/versions in new releases.
	spawnArgs.GetString( "endEoCRelease", "", endEoCRelease );
	if ( endEoCRelease != "" ) {
		idPlayer *player = gameLocal.GetLocalPlayer();
		if ( player != NULL ) {
			player->beaten=true;
		}
	}
*/
	if ( wait >= 0 ) {
		nextTriggerTime = gameLocal.time + SEC2MS( wait + random * gameLocal.random.CRandomFloat() );
	} else {
		gameLocal.SetPersistentRemove( name.c_str() );

		// we can't just remove (this) here, because this is a touch function
		// called while looping through area links...
		nextTriggerTime = gameLocal.time + 1;
		PostEventMS( &EV_Remove, 0 );
	}
}

/*
================
idTrigger_Multi::Event_TriggerAction
================
*/
void idTrigger_Multi::Event_TriggerAction( idEntity *activator ) {
	TriggerAction( activator );
}

/*
================
idTrigger_Multi::Event_Trigger

the trigger was just activated
activated should be the entity that originated the activation sequence (ie. the original target)
activator should be set to the activator so it can be held through a delay
so wait for the delay time before firing
================
*/
void idTrigger_Multi::Event_Trigger( idEntity *activator ) {
	if ( nextTriggerTime > gameLocal.time ) {
		// can't retrigger until the wait is over
		return;
	}

	// see if this trigger requires an item
	if ( !gameLocal.RequirementMet( activator, requires, removeItem ) ) {
		return;
	}

	if ( !CheckFacing( activator ) ) {
		return;
	}

	if ( triggerFirst ) {
		triggerFirst = false;
		return;
	}

	// don't allow it to trigger twice in a single frame
	nextTriggerTime = gameLocal.time + 1;

	if ( delay > 0 ) {
		// don't allow it to trigger again until our delay has passed
		nextTriggerTime += SEC2MS( delay + random_delay * gameLocal.random.CRandomFloat() );
		PostEventSec( &EV_TriggerAction, delay, activator );
	} else {
		TriggerAction( activator );
	}
}

/*
================
idTrigger_Multi::Event_Touch
================
*/
void idTrigger_Multi::Event_Touch( idEntity *other, trace_t *trace ) {
	if( triggerFirst ) {
		return;
	}

	bool player = other->IsType( idPlayer::Type );
	if ( player ) {
		if ( !touchClient ) {
			return;
		}
		if ( static_cast< idPlayer * >( other )->spectating ) {
			return;
		}
	} else if ( !touchOther ) {
		return;
	}

	if ( nextTriggerTime > gameLocal.time ) {
		// can't retrigger until the wait is over
		return;
	}

	// see if this trigger requires an item
	if ( !gameLocal.RequirementMet( other, requires, removeItem ) ) {
		return;
	}

	if ( !CheckFacing( other ) ) {
		return;
	}

	if ( spawnArgs.GetBool( "toggleTriggerFirst" ) ) {
		triggerFirst = true;
	}

	nextTriggerTime = gameLocal.time + 1;
	if ( delay > 0 ) {
		// don't allow it to trigger again until our delay has passed
		nextTriggerTime += SEC2MS( delay + random_delay * gameLocal.random.CRandomFloat() );
		PostEventSec( &EV_TriggerAction, delay, other );
	} else {
		TriggerAction( other );
	}
}

/*
===============================================================================

  idTrigger_EntityName

===============================================================================
*/

CLASS_DECLARATION( idTrigger, idTrigger_EntityName )
	EVENT( EV_Touch,			idTrigger_EntityName::Event_Touch )
	EVENT( EV_Activate,			idTrigger_EntityName::Event_Trigger )
	EVENT( EV_TriggerAction,	idTrigger_EntityName::Event_TriggerAction )
END_CLASS

/*
================
idTrigger_EntityName::idTrigger_EntityName
================
*/
idTrigger_EntityName::idTrigger_EntityName( void ) {
	wait = 0.0f;
	random = 0.0f;
	delay = 0.0f;
	random_delay = 0.0f;
	nextTriggerTime = 0;
	triggerFirst = false;
}

/*
================
idTrigger_EntityName::Save
================
*/
void idTrigger_EntityName::Save( idSaveGame *savefile ) const {
	savefile->WriteFloat( wait );
	savefile->WriteFloat( random );
	savefile->WriteFloat( delay );
	savefile->WriteFloat( random_delay );
	savefile->WriteInt( nextTriggerTime );
	savefile->WriteBool( triggerFirst );
	savefile->WriteString( entityName );
}

/*
================
idTrigger_EntityName::Restore
================
*/
void idTrigger_EntityName::Restore( idRestoreGame *savefile ) {
	savefile->ReadFloat( wait );
	savefile->ReadFloat( random );
	savefile->ReadFloat( delay );
	savefile->ReadFloat( random_delay );
	savefile->ReadInt( nextTriggerTime );
	savefile->ReadBool( triggerFirst );
	savefile->ReadString( entityName );
}

/*
================
idTrigger_EntityName::Spawn
================
*/
void idTrigger_EntityName::Spawn( void ) {
	spawnArgs.GetFloat( "wait", "0.5", wait );
	spawnArgs.GetFloat( "random", "0", random );
	spawnArgs.GetFloat( "delay", "0", delay );
	spawnArgs.GetFloat( "random_delay", "0", random_delay );

	if ( random && ( random >= wait ) && ( wait >= 0 ) ) {
		random = wait - 1;
		gameLocal.Warning( "idTrigger_EntityName '%s' at (%s) has random >= wait", name.c_str(), GetPhysics()->GetOrigin().ToString(0) );
	}

	if ( random_delay && ( random_delay >= delay ) && ( delay >= 0 ) ) {
		random_delay = delay - 1;
		gameLocal.Warning( "idTrigger_EntityName '%s' at (%s) has random_delay >= delay", name.c_str(), GetPhysics()->GetOrigin().ToString(0) );
	}

	spawnArgs.GetBool( "triggerFirst", "0", triggerFirst );

	entityName = spawnArgs.GetString( "entityname" );
	if ( !entityName.Length() ) {
		gameLocal.Error( "idTrigger_EntityName '%s' at (%s) doesn't have 'entityname' key specified", name.c_str(), GetPhysics()->GetOrigin().ToString(0) );
	}

	nextTriggerTime = 0;

	if ( !spawnArgs.GetBool( "noTouch" ) ) {
		GetPhysics()->SetContents( CONTENTS_TRIGGER );
	}
}

/*
================
idTrigger_EntityName::TriggerAction
================
*/
void idTrigger_EntityName::TriggerAction( idEntity *activator ) {
	ActivateTargets( activator );
	CallScript();

	if ( wait >= 0 ) {
		nextTriggerTime = gameLocal.time + SEC2MS( wait + random * gameLocal.random.CRandomFloat() );
	} else {
		gameLocal.SetPersistentRemove( name.c_str() );

		// we can't just remove (this) here, because this is a touch function
		// called while looping through area links...
		nextTriggerTime = gameLocal.time + 1;
		PostEventMS( &EV_Remove, 0 );
	}
}

/*
================
idTrigger_EntityName::Event_TriggerAction
================
*/
void idTrigger_EntityName::Event_TriggerAction( idEntity *activator ) {
	TriggerAction( activator );
}

/*
================
idTrigger_EntityName::Event_Trigger

the trigger was just activated
activated should be the entity that originated the activation sequence (ie. the original target)
activator should be set to the activator so it can be held through a delay
so wait for the delay time before firing
================
*/
void idTrigger_EntityName::Event_Trigger( idEntity *activator ) {
	if ( nextTriggerTime > gameLocal.time ) {
		// can't retrigger until the wait is over
		return;
	}

	if ( !activator || ( activator->name != entityName ) ) {
		return;
	}

	if ( triggerFirst ) {
		triggerFirst = false;
		return;
	}

	// don't allow it to trigger twice in a single frame
	nextTriggerTime = gameLocal.time + 1;

	if ( delay > 0 ) {
		// don't allow it to trigger again until our delay has passed
		nextTriggerTime += SEC2MS( delay + random_delay * gameLocal.random.CRandomFloat() );
		PostEventSec( &EV_TriggerAction, delay, activator );
	} else {
		TriggerAction( activator );
	}
}

/*
================
idTrigger_EntityName::Event_Touch
================
*/
void idTrigger_EntityName::Event_Touch( idEntity *other, trace_t *trace ) {
	if( triggerFirst ) {
		return;
	}

	if ( nextTriggerTime > gameLocal.time ) {
		// can't retrigger until the wait is over
		return;
	}

	if ( !other || ( other->name != entityName ) ) {
		return;
	}

	nextTriggerTime = gameLocal.time + 1;
	if ( delay > 0 ) {
		// don't allow it to trigger again until our delay has passed
		nextTriggerTime += SEC2MS( delay + random_delay * gameLocal.random.CRandomFloat() );
		PostEventSec( &EV_TriggerAction, delay, other );
	} else {
		TriggerAction( other );
	}
}

/*
===============================================================================

  idTrigger_Timer

===============================================================================
*/

const idEventDef EV_Timer( "<timer>", NULL );

CLASS_DECLARATION( idTrigger, idTrigger_Timer )
	EVENT( EV_Timer,		idTrigger_Timer::Event_Timer )
	EVENT( EV_Activate,		idTrigger_Timer::Event_Use )
END_CLASS

/*
================
idTrigger_Timer::idTrigger_Timer
================
*/
idTrigger_Timer::idTrigger_Timer( void ) {
	random = 0.0f;
	wait = 0.0f;
	on = false;
	delay = 0.0f;
}

/*
================
idTrigger_Timer::Save
================
*/
void idTrigger_Timer::Save( idSaveGame *savefile ) const {
	savefile->WriteFloat( random );
	savefile->WriteFloat( wait );
	savefile->WriteBool( on );
	savefile->WriteFloat( delay );
	savefile->WriteString( onName );
	savefile->WriteString( offName );
}

/*
================
idTrigger_Timer::Restore
================
*/
void idTrigger_Timer::Restore( idRestoreGame *savefile ) {
	savefile->ReadFloat( random );
	savefile->ReadFloat( wait );
	savefile->ReadBool( on );
	savefile->ReadFloat( delay );
	savefile->ReadString( onName );
	savefile->ReadString( offName );
}

/*
================
idTrigger_Timer::Spawn

Repeatedly fires its targets.
Can be turned on or off by using.
================
*/
void idTrigger_Timer::Spawn( void ) {
	spawnArgs.GetFloat( "random", "1", random );
	spawnArgs.GetFloat( "wait", "1", wait );
	spawnArgs.GetBool( "start_on", "0", on );
	spawnArgs.GetFloat( "delay", "0", delay );
	onName = spawnArgs.GetString( "onName" );
	offName = spawnArgs.GetString( "offName" );

	if ( random >= wait && wait >= 0 ) {
		random = wait - 0.001;
		gameLocal.Warning( "idTrigger_Timer '%s' at (%s) has random >= wait", name.c_str(), GetPhysics()->GetOrigin().ToString(0) );
	}

	if ( on ) {
		PostEventSec( &EV_Timer, delay );
	}
}

/*
================
idTrigger_Timer::Enable
================
*/
void idTrigger_Timer::Enable( void ) {
	// if off, turn it on
	if ( !on ) {
		on = true;
		PostEventSec( &EV_Timer, delay );
		gameLocal.SetPersistentTrigger( "timer", name.c_str(), true );
	}
}

/*
================
idTrigger_Timer::Disable
================
*/
void idTrigger_Timer::Disable( void ) {
	// if on, turn it off
	if ( on ) {
		on = false;
		CancelEvents( &EV_Timer );
		gameLocal.SetPersistentTrigger( "timer", name.c_str(), false );
	}
}

/*
================
idTrigger_Timer::Event_Timer
================
*/
void idTrigger_Timer::Event_Timer( void ) {
	ActivateTargets( this );

	// set time before next firing
	if ( wait >= 0.0f ) {
		PostEventSec( &EV_Timer, wait + gameLocal.random.CRandomFloat() * random );
	}
}

/*
================
idTrigger_Timer::Event_Use
================
*/
void idTrigger_Timer::Event_Use( idEntity *activator ) {
	// if on, turn it off
	if ( on ) {
		if ( offName.Length() && offName.Icmp( activator->GetName() ) ) {
			return;
		}
		on = false;
		CancelEvents( &EV_Timer );
	} else {
		// turn it on
		if ( onName.Length() && onName.Icmp( activator->GetName() ) ) {
			return;
		}
		on = true;
		PostEventSec( &EV_Timer, delay );
	}
}

/*
===============================================================================

  idTrigger_Count

===============================================================================
*/

CLASS_DECLARATION( idTrigger, idTrigger_Count )
	EVENT( EV_Activate,	idTrigger_Count::Event_Trigger )
	EVENT( EV_TriggerAction,	idTrigger_Count::Event_TriggerAction )
END_CLASS

/*
================
idTrigger_Count::idTrigger_Count
================
*/
idTrigger_Count::idTrigger_Count( void ) {
	goal = 0;
	count = 0;
	delay = 0.0f;
}

/*
================
idTrigger_Count::Save
================
*/
void idTrigger_Count::Save( idSaveGame *savefile ) const {
	savefile->WriteInt( goal );
	savefile->WriteInt( count );
	savefile->WriteFloat( delay );
}

/*
================
idTrigger_Count::Restore
================
*/
void idTrigger_Count::Restore( idRestoreGame *savefile ) {
	savefile->ReadInt( goal );
	savefile->ReadInt( count );
	savefile->ReadFloat( delay );
}

/*
================
idTrigger_Count::Spawn
================
*/
void idTrigger_Count::Spawn( void ) {
	spawnArgs.GetInt( "count", "1", goal );
	spawnArgs.GetFloat( "delay", "0", delay );
	count = 0;
}

/*
================
idTrigger_Count::Event_Trigger
================
*/
void idTrigger_Count::Event_Trigger( idEntity *activator ) {
	// goal of -1 means trigger has been exhausted
	if (goal >= 0) {
		count++;
		if ( count >= goal ) {
			if (spawnArgs.GetBool("repeat")) {
				count = 0;
			} else {
				goal = -1;
			}
			gameLocal.SetPersistentTriggerInt( "count", "count", name.c_str(), count );
			gameLocal.SetPersistentTriggerInt( "count", "goal", name.c_str(), goal );
			PostEventSec( &EV_TriggerAction, delay, activator );
		}
	}
}

/*
================
idTrigger_Count::Event_TriggerAction
================
*/
void idTrigger_Count::Event_TriggerAction( idEntity *activator ) {
	ActivateTargets( activator );
	CallScript();
	if ( goal == -1 ) {
		gameLocal.SetPersistentRemove( name.c_str() );

		PostEventMS( &EV_Remove, 0 );
	}
}

/*
===============================================================================

  idTrigger_Hurt

===============================================================================
*/

CLASS_DECLARATION( idTrigger, idTrigger_Hurt )
	EVENT( EV_Touch,		idTrigger_Hurt::Event_Touch )
	EVENT( EV_Activate,		idTrigger_Hurt::Event_Toggle )
END_CLASS


/*
================
idTrigger_Hurt::idTrigger_Hurt
================
*/
idTrigger_Hurt::idTrigger_Hurt( void ) {
	on = false;
	delay = 0.0f;
// HEXEN : Zeroth
	nextTime.Clear();
}

/*
================
idTrigger_Hurt::Save
================
*/
void idTrigger_Hurt::Save( idSaveGame *savefile ) const {
	savefile->WriteBool( on );
	savefile->WriteFloat( delay );
// HEXEN : Zeroth
	savefile->WriteInt( nextTime.Num() );
	for (int i=0; i<nextTime.Num(); i++) {
		savefile->WriteString( nextTime[i].GetString("name") );
		savefile->WriteFloat( nextTime[i].GetFloat("time") );
	}
	savefile->WriteBool(limitEntityType);
	savefile->WriteBool(dontTripby_LocalPlayer);
	savefile->WriteBool(tripby_idPlayer);
	savefile->WriteBool(tripby_idAI);
	savefile->WriteBool(tripby_idMoveable);
	savefile->WriteBool(tripby_idItem);
	savefile->WriteBool(tripby_idActor);
	savefile->WriteBool(tripby_idProjectile);
}

/*
================
idTrigger_Hurt::Restore
================
*/
void idTrigger_Hurt::Restore( idRestoreGame *savefile ) {
	savefile->ReadBool( on );
	savefile->ReadFloat( delay );
// HEXEN : Zeroth
	int c;
	float f;
	idStr s;
	savefile->ReadInt( c );
	for (int i=0; i<c; i++) {
		savefile->ReadString( s );
		savefile->ReadFloat( f );
		nextTime.Append( idDict() );
		nextTime[i].Set("name", s);
		nextTime[i].SetFloat("time", f);
	}
	savefile->ReadBool(limitEntityType);
	savefile->ReadBool(dontTripby_LocalPlayer);
	savefile->ReadBool(tripby_idPlayer);
	savefile->ReadBool(tripby_idAI);
	savefile->ReadBool(tripby_idMoveable);
	savefile->ReadBool(tripby_idItem);
	savefile->ReadBool(tripby_idActor);
	savefile->ReadBool(tripby_idProjectile);
}

/*
================
idTrigger_Hurt::Spawn

	Damages activator
	Can be turned on or off by using.
================
*/
void idTrigger_Hurt::Spawn( void ) {
	spawnArgs.GetBool( "on", "1", on );
	spawnArgs.GetFloat( "delay", "1.0", delay );
// HEXEN : Zeroth
	for (int i=0; i<nextTime.Num(); i++) {
		nextTime[i].SetFloat("time", gameLocal.time);
	}

// HEXEN : Zeroth - for limit entity types
	spawnArgs.GetBool( "dontTripby_LocalPlayer", "0", dontTripby_LocalPlayer);
	spawnArgs.GetBool( "limitEntityType", "0", limitEntityType);
	spawnArgs.GetBool( "tripby_idPlayer", "0", tripby_idPlayer);
	spawnArgs.GetBool( "tripby_idAI", "0", tripby_idAI);
	spawnArgs.GetBool( "tripby_idMoveable", "0", tripby_idMoveable);
	spawnArgs.GetBool( "tripby_idItem", "0", tripby_idItem);
	spawnArgs.GetBool( "tripby_idActor", "0", tripby_idActor);
	spawnArgs.GetBool( "tripby_idProjectile", "0", tripby_idProjectile);

	Enable();
}

/*
================
idTrigger_Hurt::Event_Touch
================
*/
void idTrigger_Hurt::Event_Touch( idEntity *other, trace_t *trace ) {
	const char *damage;
	int i;
	idStr name;
	bool located;

	if ( on && other ) {
// HEXEN : Zeroth
		if (limitEntityType) {
			if ( ( !tripby_idPlayer && other->IsType( idPlayer::Type ) ) ||
				( !tripby_idAI && other->IsType( idAI::Type ) ) ||
				( !tripby_idActor && other->IsType( idActor::Type ) ) ||
				( !tripby_idProjectile && other->IsType( idProjectile::Type ) ) ||
				( !tripby_idItem && other->IsType( idItem::Type ) ) ||
				( !tripby_idMoveable && other->IsType( idMoveable::Type ) ) ) {
				return;
			}
		}

		if ( dontTripby_LocalPlayer && other->IsType( idPlayer::Type ) && gameLocal.GetLocalPlayer() == ( static_cast<idPlayer *>( other ) ) ) {
			return;
		}

		// delay time based on individual entites
		if ( delay > 0 ) {
			name = other->GetName();
			located=false;
			for (i=0; i < nextTime.Num(); i++) {
				if ( nextTime[i].GetString( "name" ) == name ) {
					if ( nextTime[i].GetFloat( "time" ) > gameLocal.time ) {
						return;
					} else {
						nextTime[i].SetFloat( "time", gameLocal.time + SEC2MS( delay ));
						located = true;
						break;
					}
				}
			}

			if ( !located ) {
				nextTime.Append( idDict() );
				nextTime[nextTime.Num()-1].Set("name", name);
				nextTime[nextTime.Num()-1].SetFloat("time", gameLocal.time);
			}
		}

		damage = spawnArgs.GetString( "def_damage", "damage_painTrigger" );
		// HEXEN : Zeroth: added the static casts
		other->Damage( static_cast< idEntity* >( this ), static_cast< idEntity* >( this ), vec3_origin, damage, 1.0f, INVALID_JOINT, trace->c.point );

		ActivateTargets( other );
		CallScript();
	}
}

/*
================
idTrigger_Hurt::Event_Toggle
================
*/
void idTrigger_Hurt::Event_Toggle( idEntity *activator ) {
	on = !on;
	gameLocal.SetPersistentTrigger( "hurt_toggle", name.c_str(), on );
}


/*
===============================================================================

  idTrigger_Fade

===============================================================================
*/

CLASS_DECLARATION( idTrigger, idTrigger_Fade )
	EVENT( EV_Activate,		idTrigger_Fade::Event_Trigger )
END_CLASS

/*
================
idTrigger_Fade::Event_Trigger
================
*/
void idTrigger_Fade::Event_Trigger( idEntity *activator ) {
	idVec4		fadeColor;
	int			fadeTime;
	idPlayer	*player;

	player = gameLocal.GetLocalPlayer();
	if ( player ) {
		fadeColor = spawnArgs.GetVec4( "fadeColor", "0, 0, 0, 1" );
		fadeTime = SEC2MS( spawnArgs.GetFloat( "fadeTime", "0.5" ) );
		player->playerView.Fade( fadeColor, fadeTime );
		PostEventMS( &EV_ActivateTargets, fadeTime, activator );
	}
}

/*
===============================================================================

  idTrigger_Touch

===============================================================================
*/

CLASS_DECLARATION( idTrigger, idTrigger_Touch )
	EVENT( EV_Activate,		idTrigger_Touch::Event_Trigger )
END_CLASS


/*
================
idTrigger_Touch::idTrigger_Touch
================
*/
idTrigger_Touch::idTrigger_Touch( void ) {
	clipModel = NULL;
// HEXEN : Zeroth
	nextTime.Clear();
	onEntrance=false;
	touchingEntities.Clear();
	flagEntities.Clear();
	resistTimeEntities.Clear();

// HEXEN : Zeroth - for limit entity types
	dontTripby_LocalPlayer=false;
	limitEntityType=false;
	tripby_idPlayer=false;
	tripby_idAI=false;
	tripby_idMoveable=false;
	tripby_idItem=false;
	tripby_idActor=false;
	tripby_idProjectile=false;

// HEXEN : Zeroth - for trigger_hurtmulti
	hurtmulti=false;

// HEXEN : Zeroth - primarily for trigger_hurtmulti
	delay=0;
}

/*
================
idTrigger_Touch::Spawn
================
*/
void idTrigger_Touch::Spawn( void ) {
	// get the clip model
	clipModel = new idClipModel( GetPhysics()->GetClipModel() );

	// remove the collision model from the physics object
	GetPhysics()->SetClipModel( NULL, 1.0f );

	if ( spawnArgs.GetBool( "start_on" ) ) {
		BecomeActive( TH_THINK );
	}

// HEXEN : Zeroth - for delay
	spawnArgs.GetFloat( "delay", "0.0", delay );
	for (int i=0; i<nextTime.Num(); i++) {
		nextTime[i].SetFloat("time", gameLocal.time);
	}

// HEXEN : Zeroth - for trigger_enter
	spawnArgs.GetBool( "onEnter", "0", onEntrance);

// HEXEN : Zeroth - for damage_multi
	spawnArgs.GetBool( "hurtmulti", "0", hurtmulti);

// HEXEN : Zeroth - for limit entity types
	spawnArgs.GetBool( "dontTripby_LocalPlayer", "0", dontTripby_LocalPlayer);
	spawnArgs.GetBool( "limitEntityType", "0", limitEntityType);
	spawnArgs.GetBool( "tripby_idPlayer", "0", tripby_idPlayer);
	spawnArgs.GetBool( "tripby_idAI", "0", tripby_idAI);
	spawnArgs.GetBool( "tripby_idMoveable", "0", tripby_idMoveable);
	spawnArgs.GetBool( "tripby_idItem", "0", tripby_idItem);
	spawnArgs.GetBool( "tripby_idActor", "0", tripby_idActor);
	spawnArgs.GetBool( "tripby_idProjectile", "0", tripby_idProjectile);


}

/*
================
idTrigger_Touch::Save
================
*/
void idTrigger_Touch::Save( idSaveGame *savefile ) {
	savefile->WriteClipModel( clipModel );

// HEXEN : Zeroth
	savefile->WriteBool(onEntrance);
	savefile->WriteBool(limitEntityType);
	savefile->WriteBool(dontTripby_LocalPlayer);
	savefile->WriteBool(tripby_idPlayer);
	savefile->WriteBool(tripby_idAI);
	savefile->WriteBool(tripby_idMoveable);
	savefile->WriteBool(tripby_idItem);
	savefile->WriteBool(tripby_idActor);
	savefile->WriteBool(tripby_idProjectile);
	savefile->WriteBool( hurtmulti );
	savefile->WriteFloat( delay );

	savefile->WriteInt(touchingEntities.Num());
	for (int i=0; i<touchingEntities.Num(); i++) {
		savefile->WriteString(touchingEntities[i]);
		savefile->WriteBool(flagEntities[i]);
		savefile->WriteInt(resistTimeEntities[i]);
	}

	savefile->WriteInt(nextTime.Num());
	for (int i=0; i<nextTime.Num(); i++) {
		savefile->WriteString( nextTime[i].GetString("name") );
		savefile->WriteFloat( nextTime[i].GetFloat("time") );
	}
}

/*
================
idTrigger_Touch::Restore
================
*/
void idTrigger_Touch::Restore( idRestoreGame *savefile ) {
	savefile->ReadClipModel( clipModel );

// HEXEN : Zeroth
	savefile->ReadBool(onEntrance);
	savefile->ReadBool(limitEntityType);
	savefile->ReadBool(dontTripby_LocalPlayer);
	savefile->ReadBool(tripby_idPlayer);
	savefile->ReadBool(tripby_idAI);
	savefile->ReadBool(tripby_idMoveable);
	savefile->ReadBool(tripby_idItem);
	savefile->ReadBool(tripby_idActor);
	savefile->ReadBool(tripby_idProjectile);
	savefile->ReadBool( hurtmulti );
	savefile->ReadFloat( delay );

	bool bol;
	int num;
	idStr str;
	int tim;
	savefile->ReadInt(num);
	for (int i=0; i<num; i++) {
		savefile->ReadString(str);
		touchingEntities.Append(str);
		savefile->ReadBool(bol);
		flagEntities.Append(bol);
		savefile->ReadInt(tim);
		resistTimeEntities.Append(tim);
	}
	int c;
	float f;
	idStr s;
	savefile->ReadInt( c );
	for (int i=0; i<c; i++) {
		savefile->ReadString( s );
		savefile->ReadFloat( f );
		nextTime.Append( idDict() );
		nextTime[i].Set("name", s);
		nextTime[i].SetFloat("time", f);
	}
	idStr g;
}

/*
================
idTrigger_Touch::TouchEntities
================
*/
void idTrigger_Touch::TouchEntities( void ) { //Z.TODO: this is getting messy, maybe split it into separate entities?
	int numClipModels, t, i, c;
	idBounds bounds;
	idClipModel *cm, *clipModelList[ MAX_GENTITIES ];

// HEXEN : Zeroth - for trigger_hurtmulti
	const char *damage;
	idStr name;
	bool dontTrigger;

// HEXEN : Zeroth - for limit entity type
	idEntity	*entity=NULL;
	idVec3		cmOrigin;
	idMat3		cmAxis;
	cmHandle_t	cmHandle;
	idVec3		myOrigin;
	idMat3		myAxis;

	if ( clipModel == NULL ) {
		return;
	}

// HEXEN : Zeroth - for trigger_enter
	if ( onEntrance ) {
		for ( c=0; c<touchingEntities.Num(); c++ ) {
			entity = gameLocal.FindEntity(touchingEntities[c].c_str());
	// remove entities from list which are no longer touching or no longer exist
			if (!flagEntities[c] || !entity ) {
				touchingEntities.RemoveIndex(c);
				touchingEntities.Condense();
				flagEntities.RemoveIndex(c);
				flagEntities.Condense();
				c--;
			} else {
	// flag all entities as NOT touching, test if they are during the clip tests
				flagEntities[c] = false;
			}
		}
	}

	bounds.FromTransformedBounds( clipModel->GetBounds(), clipModel->GetOrigin(), clipModel->GetAxis() );
	numClipModels = gameLocal.clip.ClipModelsTouchingBounds( bounds, -1, clipModelList, MAX_GENTITIES );

	for ( i = 0; i < numClipModels; i++ ) {
		cm = clipModelList[ i ];
		if ( !cm->IsTraceModel() ) {
			continue;
		}

		entity = clipModelList[ i ]->GetEntity();
		if ( !entity ) {
			continue;
		}

		cmOrigin = cm->GetOrigin();
		cmAxis = cm->GetAxis();
		cmHandle = clipModel->Handle();
		myOrigin = clipModel->GetOrigin();
		myAxis =  clipModel->GetAxis();

		if ( !gameLocal.clip.ContentsModel( cmOrigin, cm, cmAxis, -1, cmHandle, myOrigin, myAxis ) ) {
			continue;
		}


// HEXEN : Zeroth - limit entity types
		if (limitEntityType) {
			if ( ( !tripby_idPlayer && entity->IsType( idPlayer::Type ) ) ||
				( !tripby_idAI && entity->IsType( idAI::Type ) ) ||
				( !tripby_idActor && entity->IsType( idActor::Type ) ) ||
				( !tripby_idProjectile && entity->IsType( idProjectile::Type ) ) ||
				( !tripby_idItem && entity->IsType( idItem::Type ) ) ||
				( !tripby_idMoveable && entity->IsType( idMoveable::Type ) ) ) {
				continue;
			}
		}

		if ( dontTripby_LocalPlayer && entity->IsType( idPlayer::Type ) && gameLocal.GetLocalPlayer() == ( static_cast<idPlayer *>( entity ) ) ) {
			return;
		}

// HEXEN : Zeroth - for trigger_enter: if the entity is still touching, dont trigger
		if ( onEntrance ) {
			dontTrigger=false;
			for ( c=0; c<touchingEntities.Num(); c++ ) {
				if ( touchingEntities[c] == entity->GetName() ) {
					flagEntities[c] = true; // flag it as touching
					dontTrigger = true;
					break;
				}
			}

// HEXEN : Zeroth - for trigger_enter: if the entity was not in our list, add it. else don't trigger.
			if ( !dontTrigger ) {
				touchingEntities.Append(idStr(entity->GetName()));
				flagEntities.Append(true);
			} else {
				continue;
			}
		}

// HEXEN : Zeroth - delay time based on individual entites
		if ( delay > 0 ) {
			name = entity->GetName();
			for (t=0; t < nextTime.Num(); t++) {
				if ( nextTime[t].GetString( "name" ) == name ) {
					if ( gameLocal.time < nextTime[t].GetFloat( "time" )  ) {
						dontTrigger = true;
						break;
					} else {
						dontTrigger = false;
						nextTime[t].SetFloat( "time", gameLocal.time + SEC2MS( delay ));
						break;
					}
				}
			}

			// if entity wasn't found in the list, add it
			if ( t == nextTime.Num() ) {
				nextTime.Append( idDict() );
				nextTime[t].Set("name", entity->GetName() );
				nextTime[t].SetFloat("time", gameLocal.time );
			} else if ( dontTrigger ) {
				continue;
			}
		}

// HEXEN : Zeroth - for trigger_hurtmulti
		if ( hurtmulti ) {
			damage = spawnArgs.GetString( "def_damage", "damage_painTrigger" );
			entity->Damage( static_cast< idEntity* >( this ), static_cast< idEntity* >( this ), vec3_origin, damage, 1.0f, INVALID_JOINT, idVec3(0,0,0) );
		}

		ActivateTargets( entity );

		if (scriptFunction != NULL) {
			idThread *thread = new idThread();
			thread->CallFunction( entity, scriptFunction, false );
			thread->DelayedStart( 0 );
		}
	}
}

/*
================
idTrigger_Touch::Think
================
*/
void idTrigger_Touch::Think( void ) {
	if ( thinkFlags & TH_THINK ) {
		TouchEntities();
	}
	idEntity::Think();
}

/*
================
idTrigger_Touch::Event_Trigger
================
*/
void idTrigger_Touch::Event_Trigger( idEntity *activator ) {
	if ( thinkFlags & TH_THINK ) {
		BecomeInactive( TH_THINK );
	} else {
		BecomeActive( TH_THINK );
	}
}

/*
================
idTrigger_Touch::Enable
================
*/
void idTrigger_Touch::Enable( void ) {
	BecomeActive( TH_THINK );
	gameLocal.SetPersistentTrigger( "touch", name.c_str(), true );
}

/*
================
idTrigger_Touch::Disable
================
*/
void idTrigger_Touch::Disable( void ) {
	BecomeInactive( TH_THINK );
	gameLocal.SetPersistentTrigger( "touch", name.c_str(), false );
}
