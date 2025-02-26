// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moaicore/MOAIAnimCurve.h>
#include <moaicore/MOAILogMessages.h>
#include <moaicore/MOAISim.h>
#include <moaicore/MOAITimer.h>

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@name	getTime
	@text	Return the current time.

	@in		MOAITimer self
	@out	number time
*/
int MOAITimer::_getTime( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAITimer, "U" )

	lua_pushnumber ( L, self->mTime );
	return 1;
}

//----------------------------------------------------------------//
/**	@name	getTimesExecuted
	@text	Gets the number of times the timer has completed a cycle.

	@in		MOAITimer self
	@out	number nTimes
*/
int MOAITimer::_getTimesExecuted ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAITimer, "UN" )

	lua_pushnumber ( L, self->mTimesExecuted );
	return 1;
}

//----------------------------------------------------------------//
/**	@name	setCurve
	@text	Set or clear the curve to use for event generation.
	
	@in		MOAIAnimCurveListener self
	@opt	MOAIAnimCurveListener curve		Default value is nil.
	@out	nil
*/
int MOAITimer::_setCurve ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAITimer, "U" );

	self->mCurve.Set ( *self, state.GetLuaObject < MOAIAnimCurve >( 2 ));
	self->ScheduleUpdate ();

	return 0;
}

//----------------------------------------------------------------//
/**	@name	setMode
	@text	Sets the playback mode of the timer.

	@in		MOAITimer self
	@in		number mode		One of: MOAITimer.NORMAL, MOAITimer.REVERSE, MOAITimer.LOOP, MOAITimer.LOOP_REVERSE, MOAITimer.PING_PONG
	@out	nil
*/
int MOAITimer::_setMode ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAITimer, "UN" )

	self->mMode = state.GetValue < int >( 2, NORMAL );
	
	if( self->mMode == REVERSE ||
		self->mMode == LOOP_REVERSE ){
		self->mDirection = -1.0f;
	}
	else {
		self->mDirection = 1.0f;
	}
	return 0;
}

//----------------------------------------------------------------//
/**	@name	setSpan
	@text	Sets the playback mode of the timer.

	@overload	Span will be 0 to endTime.

		@in		MOAITimer self
		@in		number endTime
		@out	nil
	
	@overload	Span will be startTime to endTime.
	
		@in		MOAITimer self
		@in		number startTime
		@in		number endTime
		@out	nil
*/
int MOAITimer::_setSpan ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAITimer, "UN" )

	if ( state.IsType ( 3, LUA_TNUMBER )) {
		self->mStartTime = state.GetValue < float >( 2, 0.0f );
		self->mEndTime = state.GetValue < float >( 3, 1.0f );
	}
	else {
		self->mStartTime = 0.0f;
		self->mEndTime = state.GetValue < float >( 2, 1.0f );
	}
	return 0;
}

//----------------------------------------------------------------//
/**	@name	setSpeed
	@text	Sets the playback speed. This affects only the timer, not
			its children in the action tree.

	@in		MOAITimer self
	@in		number speed
	@out	nil
*/
int MOAITimer::_setSpeed ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAITimer, "UN" )

	self->mSpeed = state.GetValue < float >( 2, 1.0f );

	return 0;
}

//----------------------------------------------------------------//
/**	@name	setTime
	@text	Manually set the current time. This will be wrapped
			into the current span.

	@in		MOAITimer self
	@opt	number time			Default value is 0.
	@out	nil
*/
int MOAITimer::_setTime ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAITimer, "U" )
	
	float time = state.GetValue < float >( 2, 0.0f );
	self->SetTime ( time );
	
	return 0;
}

//================================================================//
// MOAITimer
//================================================================//

//----------------------------------------------------------------//
bool MOAITimer::ApplyAttrOp ( u32 attrID, MOAIAttrOp& attrOp, u32 op ) {

	if ( MOAITimerAttr::Check ( attrID )) {
		attrID = UNPACK_ATTR ( attrID );
		
		if ( attrID == ATTR_TIME ) {
			this->mTime = attrOp.Apply ( this->mTime, op, MOAINode::ATTR_READ_WRITE );
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------//
float MOAITimer::DoStep ( float step ) {

	float length = this->mEndTime - this->mStartTime;

	if ( length == 0.0f ) {
		this->Stop ();
		this->ScheduleUpdate ();
		return 0.0f;
	}

	float t0 = this->mTime;
	this->mTime += step * this->mSpeed * this->mDirection;
	
	float t1 = this->mTime;
	float result = 0.0f;
	
	switch ( this->mMode ) {
	
		case NORMAL: {
			
			if ( this->mTime >= this->mEndTime ) {
				this->mTime = this->mEndTime;
				this->GenerateCallbacks ( t0, this->mTime, true );
				this->OnLoop ();
				this->Stop ();
			}
			else {
				this->GenerateCallbacks ( t0, this->mTime, false );
			}
			result = this->mTime - t0;
			break;
		}
		
		case LOOP: {
			
			if ( this->mTime >= this->mEndTime ) {
				
				this->GenerateCallbacks ( t0, this->mEndTime, true );
				
				while ( this->mTime >= this->mEndTime ) {
					this->mTime -= length;
					this->OnLoop ();
					
					float end = this->mTime < this->mEndTime ? this->mTime : this->mEndTime;
					this->GenerateCallbacks ( this->mStartTime, end, end == this->mEndTime );
				}
			}
			else {
				this->GenerateCallbacks ( t0, this->mTime, false );
			}
			result = t1 - t0;
			break;
		}
		
		case REVERSE: {
		
			if ( this->mTime < this->mStartTime ) {
				this->mTime = this->mStartTime ;
				this->GenerateCallbacks ( t0, this->mTime, true );
				this->OnLoop ();
				this->Stop ();
			}
			else {
				this->GenerateCallbacks ( t0, this->mTime, false );
			}
			result = this->mTime - t0;
			break;
		}
		
		case LOOP_REVERSE: {
		
			if ( this->mTime <= this->mStartTime ) {
				
				this->GenerateCallbacks ( t0, this->mStartTime, true );
				
				while ( this->mTime <= this->mStartTime ) {
					this->mTime += length;
					this->OnLoop ();
					
					float end = this->mTime > this->mStartTime ? this->mTime : this->mStartTime;
					this->GenerateCallbacks ( this->mEndTime, end, end == this->mStartTime );
				}
			}
			else {
				this->GenerateCallbacks ( t0, this->mTime, false );
			}
			result = t1 - t0;
			break;
		}
		
		case PING_PONG: {
			
			if (( this->mTime < this->mStartTime ) || ( this->mTime >= this->mEndTime )) {
			
				while (( this->mTime < this->mStartTime ) || ( this->mTime >= this->mEndTime )) {
					
					if ( this->mTime < this->mStartTime ) {
						this->mTime = this->mStartTime + ( this->mStartTime - this->mTime );
						
						float end = this->mTime < this->mEndTime ? this->mTime : this->mEndTime;
						this->GenerateCallbacks ( this->mStartTime, end, end == this->mEndTime );
					}
					else {
						this->mTime = this->mEndTime - ( this->mTime - this->mEndTime );
						
						float end = this->mTime > this->mStartTime ? this->mTime : this->mStartTime;
						this->GenerateCallbacks ( this->mEndTime, end, end == this->mStartTime );
					}
					
					this->mDirection *= -1.0f;
					this->OnLoop ();
				}
			}
			else {
				this->GenerateCallbacks ( t0, this->mTime, false );
			}
			result = this->mTime - t0;
			break;
		}
	}
	
	this->ScheduleUpdate ();
	return result;
}

//----------------------------------------------------------------//
void MOAITimer::GenerateCallbacks ( float t0, float t1, bool end ) {

	u32 size = this->mCurve ? this->mCurve->Size () : 0;
	
	if ( size ) {
		
		if ( t0 != t1 ) {
			
			u32 keyID = ( int )this->mCurve->FindKeyID ( t0 );
			
			if ( t0 < t1 ) {
			
				for ( ; keyID < size; ++keyID ) {
					MOAIAnimKey& key = ( *this->mCurve )[ keyID ];
					
					if (( end && ( key.mTime >= t1 )) || (( key.mTime >= t0 ) && ( key.mTime < t1 ))) {
						this->OnKeyframe ( keyID, key.mTime, key.mValue );
					}
					
					if ( key.mTime >= t1 ) break;
				}
			}
			else {
			
				for ( ; ( int )keyID >= -1; --keyID ) {
					MOAIAnimKey& key = ( *this->mCurve )[ keyID ];
				
					if (( end && ( key.mTime <= t1 )) || (( key.mTime <= t0 ) && ( key.mTime > t1 ))) {
						this->OnKeyframe ( keyID, key.mTime, key.mValue );
					}
					
					if ( key.mTime <= t1 ) break;
				}
			}
		}
	}
}

//----------------------------------------------------------------//
bool MOAITimer::IsDone () {

	if ( this->mMode == NORMAL ) {
		return (( this->mTime < this->mStartTime ) || ( this->mTime >= this->mEndTime ));
	}
	
	if ( this->mMode == REVERSE ) {
		return (( this->mTime <= this->mStartTime ) || ( this->mTime > this->mEndTime ));
	}
	
	return false;
}

//----------------------------------------------------------------//
MOAITimer::MOAITimer () :
	mStartTime ( 0.0f ),
	mEndTime ( 1.0f ),
	mTime ( 0.0f ),
	mSpeed ( 1.0f ),
	mDirection ( 1.0f ),
	mMode ( NORMAL ),
	mTimesExecuted ( 0 ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAINode )
		RTTI_EXTEND ( MOAIAction )
	RTTI_END
}

//----------------------------------------------------------------//
MOAITimer::~MOAITimer () {

	this->mCurve.Set ( *this, 0 );
}

//----------------------------------------------------------------//
void MOAITimer::OnDepNodeUpdate () {
}

//----------------------------------------------------------------//
void MOAITimer::OnKeyframe ( u32 idx, float time, float value ) {

	MOAILuaStateHandle state = MOAILuaRuntime::Get ().State ();
	if ( this->PushListenerAndSelf ( EVENT_TIMER_KEYFRAME, state )) {
		state.Push ( idx + 1 );
		state.Push ( time );
		state.Push ( value );
		state.DebugCall ( 4, 0 );
	}
}

//----------------------------------------------------------------//
void MOAITimer::OnLoop () {
	
	this->mTimesExecuted++;
	
	MOAILuaStateHandle state = MOAILuaRuntime::Get ().State ();
	if ( this->PushListenerAndSelf ( EVENT_TIMER_LOOP, state )) {
		state.Push ( this->mTimesExecuted );
		state.DebugCall ( 2, 0 );
	}
}

//----------------------------------------------------------------//
void MOAITimer::OnStart () {

	if( this->mDirection > 0.0f ) {
		this->mTime = this->mStartTime;
	}
	else {
		this->mTime = this->mEndTime;
	}
}

//----------------------------------------------------------------//
void MOAITimer::OnUpdate ( float step ) {

	this->DoStep ( step );
}

//----------------------------------------------------------------//
void MOAITimer::RegisterLuaClass ( MOAILuaState& state ) {

	MOAINode::RegisterLuaClass ( state );
	MOAIAction::RegisterLuaClass ( state );

	state.SetField ( -1, "ATTR_TIME", MOAITimerAttr::Pack ( ATTR_TIME ));
	
	state.SetField ( -1, "EVENT_TIMER_KEYFRAME", ( u32 )EVENT_TIMER_KEYFRAME );
	state.SetField ( -1, "EVENT_TIMER_LOOP", ( u32 )EVENT_TIMER_LOOP );
	
	state.SetField ( -1, "NORMAL", ( u32 )NORMAL );
	state.SetField ( -1, "REVERSE", ( u32 )REVERSE );
	state.SetField ( -1, "LOOP", ( u32 )LOOP );
	state.SetField ( -1, "LOOP_REVERSE", ( u32 )LOOP_REVERSE );
	state.SetField ( -1, "PING_PONG", ( u32 )PING_PONG );
}

//----------------------------------------------------------------//
void MOAITimer::RegisterLuaFuncs ( MOAILuaState& state ) {

	MOAINode::RegisterLuaFuncs ( state );
	MOAIAction::RegisterLuaFuncs ( state );

	luaL_Reg regTable [] = {
		{ "getTime",			_getTime },
		{ "getTimesExecuted",	_getTimesExecuted },
		{ "setCurve",			_setCurve },
		{ "setMode",			_setMode },
		{ "setSpan",			_setSpan },
		{ "setSpeed",			_setSpeed },
		{ "setTime",			_setTime },
		{ NULL, NULL }
	};

	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAITimer::SetTime ( float time ) {

	float length = USFloat::Abs ( this->mEndTime - this->mStartTime );
	while ( time >= this->mEndTime ) {
		time -= length;
	}
	this->mTime = time;
	this->mTimesExecuted = 0;
	this->ScheduleUpdate ();
}

