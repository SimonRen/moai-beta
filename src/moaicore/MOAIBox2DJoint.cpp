// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <Box2D/Box2D.h>
#include <moaicore/MOAIBox2DArbiter.h>
#include <moaicore/MOAIBox2DBody.h>
#include <moaicore/MOAIBox2DJoint.h>
#include <moaicore/MOAIBox2DWorld.h>
#include <moaicore/MOAILogMessages.h>
#include <moaicore/MOAISim.h>

SUPPRESS_EMPTY_FILE_WARNING
#if USE_BOX2D

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@name	destroy
	@text	Schedule joint for destruction.
	
	@in		MOAIBox2DBody self
	@out	nil
*/
int MOAIBox2DJoint::_destroy ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIBox2DJoint, "U" )
	
	if ( self->mWorld ) {
		self->mWorld->ScheduleDestruction ( *self );
	}
	return 0;
}

//----------------------------------------------------------------//
/**	@name	getAnchorA
	@text	See Box2D documentation.
	
	@in		MOAIBox2DJoint self
	@out	anchorX
	@out	anchorY
*/
int MOAIBox2DJoint::_getAnchorA ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIBox2DJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mJoint ) {
		MOAILog ( state, MOAILogMessages::MOAIBox2DJoint_MissingInstance );
		return 0;
	}
	
	b2Vec2 anchor = self->mJoint->GetAnchorA ();
	lua_pushnumber ( state, anchor.x / unitsToMeters );
	lua_pushnumber ( state, anchor.y / unitsToMeters );
	
	return 2;
}

//----------------------------------------------------------------//
/**	@name	getAnchorB
	@text	See Box2D documentation.
	
	@in		MOAIBox2DJoint self
	@out	anchorX
	@out	anchorY
*/
int MOAIBox2DJoint::_getAnchorB ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIBox2DJoint, "U" )
	float unitsToMeters = self->GetUnitsToMeters ();
	
	if ( !self->mJoint ) {
		MOAILog ( state, MOAILogMessages::MOAIBox2DJoint_MissingInstance );
		return 0;
	}
	
	b2Vec2 anchor = self->mJoint->GetAnchorB ();
	lua_pushnumber ( state, anchor.x / unitsToMeters );
	lua_pushnumber ( state, anchor.y / unitsToMeters );
	
	return 2;
}

//----------------------------------------------------------------//
/**	@name	getBodyA
	@text	See Box2D documentation.
	
	@in		MOAIBox2DJoint self
	@out	MOAIBox2DBody body
*/
int MOAIBox2DJoint::_getBodyA ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIBox2DJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILog ( state, MOAILogMessages::MOAIBox2DJoint_MissingInstance );
		return 0;
	}
	
	MOAIBox2DBody* body = ( MOAIBox2DBody* )self->mJoint->GetBodyA ()->GetUserData ();
	body->PushLuaUserdata ( state );
	
	return 1;
}

//----------------------------------------------------------------//
/**	@name	getBodyB
	@text	See Box2D documentation.
	
	@in		MOAIBox2DJoint self
	@out	MOAIBox2DBody body
*/
int MOAIBox2DJoint::_getBodyB ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIBox2DJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILog ( state, MOAILogMessages::MOAIBox2DJoint_MissingInstance );
		return 0;
	}
	
	MOAIBox2DBody* body = ( MOAIBox2DBody* )self->mJoint->GetBodyB ()->GetUserData ();
	body->PushLuaUserdata ( state );
	
	return 1;
}

//----------------------------------------------------------------//
/**	@name	getReactionForce
	@text	See Box2D documentation.
	
	@in		MOAIBox2DJoint self
	@out	number forceX
	@out	number forceY
*/
int MOAIBox2DJoint::_getReactionForce ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIBox2DJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILog ( state, MOAILogMessages::MOAIBox2DJoint_MissingInstance );
		return 0;
	}
	
	float step = ( float )( 1.0 / MOAISim::Get ().GetStep ());
	
	b2Vec2 force = self->mJoint->GetReactionForce ( step );
	lua_pushnumber ( state, force.x );
	lua_pushnumber ( state, force.y );
	
	return 2;
}

//----------------------------------------------------------------//
/**	@name	getReactionForce
	@text	See Box2D documentation.
	
	@in		MOAIBox2DJoint self
	@out	number reactionTorque In degrees.
*/
int MOAIBox2DJoint::_getReactionTorque ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIBox2DJoint, "U" )
	
	if ( !self->mJoint ) {
		MOAILog ( state, MOAILogMessages::MOAIBox2DJoint_MissingInstance );
		return 0;
	}
	
	float step = ( float )( 1.0 / MOAISim::Get ().GetStep ());
	
	float torque = self->mJoint->GetReactionTorque ( step );
	lua_pushnumber ( state, torque );
	
	return 1;
}

//================================================================//
// MOAIBox2DJoint
//================================================================//

//----------------------------------------------------------------//
void MOAIBox2DJoint::Destroy () {

	if ( this->mJoint ) {
		b2World* world = this->mWorld->mWorld;
		world->DestroyJoint ( this->mJoint );
		this->mJoint = 0;
	}
}

//----------------------------------------------------------------//
MOAIBox2DJoint::MOAIBox2DJoint () :
	mJoint ( 0 ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAILuaObject )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIBox2DJoint::~MOAIBox2DJoint () {

	if ( this->mJoint ) {

		MOAIBox2DBody* bodyA = ( MOAIBox2DBody* )this->mJoint->GetBodyA ();
		MOAIBox2DBody* bodyB = ( MOAIBox2DBody* )this->mJoint->GetBodyB ();
	
		bodyA->Release ();
		bodyB->Release ();
	}
	
	this->Destroy ();
}

//----------------------------------------------------------------//
void MOAIBox2DJoint::RegisterLuaClass ( MOAILuaState& state ) {
	MOAIBox2DPrim::RegisterLuaClass ( state );
}

//----------------------------------------------------------------//
void MOAIBox2DJoint::RegisterLuaFuncs ( MOAILuaState& state ) {
	MOAIBox2DPrim::RegisterLuaFuncs ( state );

	luaL_Reg regTable [] = {
		{ "destroy",				_destroy },
		{ "getAnchorA",				_getAnchorA },
		{ "getAnchorB",				_getAnchorB },
		{ "getBodyA",				_getBodyA },
		{ "getBodyB",				_getBodyB },
		{ "getReactionForce",		_getReactionForce },
		{ "getReactionTorque",		_getReactionTorque },
		{ NULL, NULL }
	};
	
	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAIBox2DJoint::SetJoint ( b2Joint* joint ) {

	this->mJoint = joint;
	joint->SetUserData ( this );
}

#endif