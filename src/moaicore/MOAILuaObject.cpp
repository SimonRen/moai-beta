// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moaicore/MOAILuaObject.h>
#include <moaicore/MOAILuaRef.h>
#include <moaicore/MOAILuaState.h>
#include <moaicore/MOAILuaRuntime.h>
#include <moaicore/MOAISerializer.h>
#include <moaicore/MOAILuaState-impl.h>

#define LUA_MEMBER_TABLE_NAME "_m"

//================================================================//
// MOAILuaLocal
//================================================================//
	
//----------------------------------------------------------------//
MOAILuaLocal::MOAILuaLocal () :
	mRef ( LUA_NOREF ) {
}

//----------------------------------------------------------------//
MOAILuaLocal::~MOAILuaLocal () {
}

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
int MOAILuaObject::_gc ( lua_State* L ) {

	MOAILuaState state ( L );
	
	MOAILuaObject* data = ( MOAILuaObject* )state.GetPtrUserData ( 1 );

	bool cleanup = ( data->GetRefCount () == 0 ); // ready to cleanup if no references

	// in any event, let's get rid of the userdata and lua refs we know about
	data->ClearLocal ( data->mContain );
	data->mUserdata.Clear ();
	data->mInstanceTable.Clear ();
	
	// check to see if gc is being invoked during finalization
	if ( MOAILuaRuntime::IsValid ()) {
		MOAILuaRuntime::Get ().ClearObjectStackTrace ( data );
	}

	// delete if no references
	if ( cleanup ) {
		delete data;
	}
	return 0;
}

//----------------------------------------------------------------//
int MOAILuaObject::_getClass ( lua_State* L ) {

	MOAILuaState state ( L );
	MOAILuaObject* object = ( MOAILuaObject* )state.GetPtrUserData ( 1 );

	if ( object ) {
		object->PushLuaClassTable ( state );
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------//
int MOAILuaObject::_getClassName ( lua_State* L ) {

	MOAILuaState state ( L );
	MOAILuaObject* object = ( MOAILuaObject* )state.GetPtrUserData ( 1 );

	if ( object ) {
		lua_pushstring ( L, object->TypeName ());
		return 1;
	}
	return 0;
}

//----------------------------------------------------------------//
int MOAILuaObject::_index ( lua_State* L ) {

	// push the instance table
	lua_getmetatable ( L, 1 );

	// push the member table
	lua_pushstring ( L, LUA_MEMBER_TABLE_NAME );
	lua_rawget ( L, -2 );
	
	// try to get the value
	lua_pushvalue ( L, 2 ); 
	lua_gettable ( L, -2 );
	
	// if nil...
	if ( lua_isnil ( L, -1 )) {
	
		// pop the nil and the member table
		lua_pop ( L, 2 );
		
		// get the instance table's metatable (the interface table)
		lua_getmetatable ( L, -1 );
		
		// and try to get the value from the interface table directly
		lua_pushvalue ( L, 2 );
		lua_rawget ( L, -2 );
	}
	return 1;
}

//----------------------------------------------------------------//
int MOAILuaObject::_newindex ( lua_State* L ) {

	// push the instance table
	lua_getmetatable ( L, 1 );

	// push the private table
	lua_pushstring ( L, LUA_MEMBER_TABLE_NAME );
	lua_rawget ( L, -2 );

	// set the index into the private table
	lua_pushvalue ( L, 2 );
	lua_pushvalue ( L, 3 );
	
	lua_settable ( L, -3 );

	return 0;
}

//----------------------------------------------------------------//
int MOAILuaObject::_tombstone ( lua_State* L ) {

	MOAILuaState state ( L );
	
	USLog::Print ( "----------------------------------------------------------------\n" );
	USLog::Print ( "ERROR: Attempt to access missing object instance.\n" );
	state.PrintStackTrace ( USLog::CONSOLE, 0 );
	USLog::Print ( "\n" );
	
	return 0;
}

//----------------------------------------------------------------//
int MOAILuaObject::_tostring ( lua_State* L ) {

	MOAILuaState state ( L );

	MOAILuaObject* data = ( MOAILuaObject* )state.GetPtrUserData ( 1 );
	if ( data ) {
	
		STLString str;
		str.write ( "0x%p <%s>", data, data->TypeName ()); // TODO: 64-bit
		state.Push ( str );
		return 1;
	}
	return 0;
}

//================================================================//
// MOAILuaObject
//================================================================//

//----------------------------------------------------------------//
void MOAILuaObject::BindToLuaWithTable ( MOAILuaState& state ) {

	assert ( !this->mUserdata );
	assert ( state.IsType ( -1, LUA_TTABLE ));
	
	MOAILuaClass* type = this->GetLuaClass ();
	assert ( type );
	
	// create and initialize a new userdata
	state.PushPtrUserData ( this );
	
	// create and initialize the private table
	lua_newtable ( state );
	
	// set the ref to the private table
	lua_pushvalue ( state, -3 );
	lua_setfield ( state, -2, LUA_MEMBER_TABLE_NAME );
	
	// initialize the private table
	lua_pushcfunction ( state, MOAILuaObject::_gc );
	lua_setfield ( state, -2, "__gc" );
	
	lua_pushcfunction ( state, MOAILuaObject::_tostring );
	lua_setfield ( state, -2, "__tostring" );
	
	lua_pushcfunction ( state, MOAILuaObject::_index );
	lua_setfield ( state, -2, "__index" );
	
	lua_pushcfunction ( state, MOAILuaObject::_newindex );
	lua_setfield ( state, -2, "__newindex" );
	
	// make the interface table the instance table's meta
	type->PushInterfaceTable ( state );
	lua_setmetatable ( state, -2 );
	
	// grab a ref to the instance table; attach it to the userdata
	this->mInstanceTable = state.GetWeakRef ( -1 );
	lua_setmetatable ( state, -2 );
	
	// and take a ref back to the userdata	
	if ( this->GetRefCount () == 0 ) {
		this->mUserdata.SetWeakRef ( state, -1 );
	}
	else {
		this->mUserdata.SetStrongRef ( state, -1 );
	}
	
	// overwrite the member table
	lua_replace ( state, -2 );
	
	assert ( !lua_isnil ( state, -1 ));
}

//----------------------------------------------------------------//
void MOAILuaObject::ClearLocal ( MOAILuaLocal& ref ) {
	
	if ( MOAILuaRuntime::IsValid ()) {
		MOAILuaStateHandle state = MOAILuaRuntime::Get ().State ();
		
		if ( this->mInstanceTable.PushRef ( state )) {
		
			lua_pushnumber ( state, ref.mRef );
			lua_pushnil ( state );
			lua_settable ( state, -3 );
			lua_pop ( state, 1 );
		}
	}
	ref.mRef = LUA_NOREF;
}

//----------------------------------------------------------------//
MOAILuaClass* MOAILuaObject::GetLuaClass () {

	// no implementation
	assert ( false );
	return 0;
}

//----------------------------------------------------------------//
MOAILuaStateHandle MOAILuaObject::GetSelf () {

	MOAILuaStateHandle state = MOAILuaRuntime::Get ().State ();
	this->PushLuaUserdata ( state );
	return state;
}

//----------------------------------------------------------------//
bool MOAILuaObject::IsBound () {

	return ( this->mUserdata != 0 );
}

//----------------------------------------------------------------//
void MOAILuaObject::LockToRefCount () {

	this->mUserdata.MakeStrong ();
}

//----------------------------------------------------------------//
void MOAILuaObject::LuaRelease ( MOAILuaObject& object ) {

	if ( this->mContain && MOAILuaRuntime::IsValid ()) {
	
		MOAILuaStateHandle state = MOAILuaRuntime::Get ().State ();
	
		if ( this->PushLocal ( state, this->mContain )) {
			
			object.PushLuaUserdata ( state );
			lua_pushnil ( state );
			lua_rawset ( state, -3 );
		}
		lua_pop ( state, 1 );
	}
	object.Release ();
}

//----------------------------------------------------------------//
void MOAILuaObject::LuaRetain ( MOAILuaObject& object ) {

	if ( this->mInstanceTable ) {
		MOAILuaStateHandle state = MOAILuaRuntime::Get ().State ();

		// affirm container table
		if ( this->mContain ) {
			this->PushLocal ( state, this->mContain );
		}
		else {
			lua_newtable ( state );
			this->SetLocal ( state, -1, this->mContain );
		}
		
		lua_pop ( state, 1 );
		this->PushLocal ( state, this->mContain );
		
		object.PushLuaUserdata ( state );
		lua_pushvalue ( state, -1 );
		lua_rawset ( state, -3 );
		
		lua_pop ( state, 1 );
	}
	object.Retain ();
}

//----------------------------------------------------------------//
void MOAILuaObject::LuaUnbind () {
	
	if ( this->mUserdata && MOAILuaRuntime::IsValid ()) {
		
		MOAILuaStateHandle state = MOAILuaRuntime::Get ().State ();
		
		this->mUserdata.PushRef ( state );
		
		void* userdata = lua_touserdata ( state, -1 );
		memset ( userdata, 0, sizeof ( void* ));
		
		lua_newtable ( state );
		
		lua_pushvalue ( state, -1 );
		lua_setmetatable ( state, -2 );
		
		lua_pushcfunction ( state, MOAILuaObject::_tombstone );
		lua_setfield ( state, -2, "__index" );
		
		lua_pushcfunction ( state, MOAILuaObject::_tombstone );
		lua_setfield ( state, -2, "__newindex" );
		
		lua_pushcfunction ( state, MOAILuaObject::_tombstone );
		lua_setfield ( state, -2, "__tostring" );
		
		lua_setmetatable ( state, -2 );

		lua_pop ( state, 1 );

		this->mUserdata.Clear ();
	}
}

//----------------------------------------------------------------//
void MOAILuaObject::OnRelease ( u32 refCount ) {

	if ( refCount == 0 ) {
	
		if ( this->mUserdata ) {
			this->mUserdata.MakeWeak ();
		}
		else {
			// no Lua binding and no references, so
			// go ahead and kill this turkey
			delete this;
		}
	}
}

//----------------------------------------------------------------//
void MOAILuaObject::OnRetain ( u32 refCount ) {
	UNUSED ( refCount );
}

//----------------------------------------------------------------//
void MOAILuaObject::PushLuaClassTable ( MOAILuaState& state ) {

	MOAILuaClass* luaType = this->GetLuaClass ();
	luaType->mClassTable.PushRef ( state );
}

//----------------------------------------------------------------//
void MOAILuaObject::PushLuaUserdata ( MOAILuaState& state ) {

	bool hasUserdata = !this->mUserdata.IsNil ();

	// create the handle userdata for reference counting
	if ( !this->mUserdata.PushRef ( state )) {
		
		// pop the 'nil' pushed by PushRef
		state.Pop ( 1 );
		
		// this is a nasty edge case where the userdata has been tagged for garbage
		// collection, but not actually collected. the result is that the ref hasn't
		// be cleared yet, but when we push it we get nil. this should only happen to
		// refs to userdata. it's tempting to try and clear out the ref here, but if
		// the ref is to a MOAILuaObject's userdata, the next step may be to recreate
		// the object... which means when it is garbage collected the wrong (new)
		// userdata will be cleaned up! so all we can do is force a full collection
		// step, set ourselves to nil and return failure.
		if ( hasUserdata ) {
			USLog::Print ( "Attempt to access MOAILuaObject userdata tagged for garbage collection; running a full cycle of GC prior to reallocation.\n" );
			MOAILuaRuntime::Get ().ForceGarbageCollection ();
		}
		
		// create an empty instance table
		lua_newtable ( state );
		
		// initialize and bind the userdata
		this->BindToLuaWithTable ( state );
	}
	assert ( !lua_isnil ( state, -1 ));
}

//----------------------------------------------------------------//
bool MOAILuaObject::PushLocal ( MOAILuaState& state, MOAILuaLocal& ref ) {

	if ( ref ) {
		
		assert ( this->mInstanceTable );
		
		this->mInstanceTable.PushRef ( state );
		lua_rawgeti ( state, -1, ref.mRef );
		lua_replace ( state, -2 );
		return true;
	}
	lua_pushnil ( state );
	return false;
}

//----------------------------------------------------------------//
void MOAILuaObject::PushMemberTable ( MOAILuaState& state ) {

	this->mInstanceTable.PushRef ( state );
	
	lua_pushstring ( state, LUA_MEMBER_TABLE_NAME );
	lua_rawget ( state, -2 );
	lua_remove ( state, -2 );
}

//----------------------------------------------------------------//
void MOAILuaObject::RegisterLuaClass ( MOAILuaState& state ) {
	UNUSED ( state );
}

//----------------------------------------------------------------//
void MOAILuaObject::RegisterLuaFuncs ( MOAILuaState& state ) {
	UNUSED ( state );
}

//----------------------------------------------------------------//
void MOAILuaObject::SerializeIn ( MOAILuaState& state, MOAIDeserializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
void MOAILuaObject::SerializeOut ( MOAILuaState& state, MOAISerializer& serializer ) {
	UNUSED ( state );
	UNUSED ( serializer );
}

//----------------------------------------------------------------//
void MOAILuaObject::SetLocal ( MOAILuaState& state, int idx, MOAILuaLocal& ref ) {

	idx = state.AbsIndex ( idx );

	assert ( this->mInstanceTable );

	this->mInstanceTable.PushRef ( state );
	
	if ( ref ) {
		luaL_unref ( state, -1, ref.mRef );
		ref.mRef = LUA_NOREF;
	}
	
	state.CopyToTop ( idx );
	ref.mRef = luaL_ref ( state, -2 );
	
	lua_pop ( state, 1 );
}

//----------------------------------------------------------------//
void MOAILuaObject::SetMemberTable ( MOAILuaState& state, int idx ) {

	this->mInstanceTable.PushRef ( state );
	
	lua_pushstring ( state, LUA_MEMBER_TABLE_NAME );
	lua_pushvalue ( state, idx );
	lua_rawset ( state, -3 );
	
	lua_pop ( state, 1 );
}

//----------------------------------------------------------------//
MOAILuaObject::MOAILuaObject () {
	RTTI_SINGLE ( RTTIBase )
	
	if ( MOAILuaRuntime::IsValid ()) {
		MOAILuaRuntime::Get ().RegisterObject ( *this );
	}
}

//----------------------------------------------------------------//
MOAILuaObject::~MOAILuaObject () {
	
	if ( MOAILuaRuntime::IsValid ()) {
		
		MOAILuaRuntime::Get ().ClearObjectStackTrace ( this );
		
		this->LuaUnbind ();
		
		MOAILuaRuntime::Get ().DeregisterObject ( *this );
	}
}

//================================================================//
// MOAILuaClass
//================================================================//

//----------------------------------------------------------------//
MOAILuaObject* MOAILuaClass::GetSingleton () {
	return 0;
}

//----------------------------------------------------------------//
void MOAILuaClass::InitLuaFactoryClass ( MOAILuaObject& data, MOAILuaState& state ) {

	int top = lua_gettop ( state );

	lua_newtable ( state );
	
	lua_pushcfunction ( state, MOAILuaObject::_getClass );
	lua_setfield ( state, -2, "getClass" );
	
	lua_pushcfunction ( state, MOAILuaObject::_getClassName );
	lua_setfield ( state, -2, "getClassName" );

	data.RegisterLuaFuncs ( state );

	lua_pushvalue ( state, -1 );
	lua_setfield ( state, -2, "__index" );

	lua_pushnil ( state );
	lua_setfield ( state, -2, "__newindex" );

	this->mInterfaceTable = state.GetStrongRef ( -1 );
	
	lua_settop ( state, top );

	lua_newtable ( state );
	this->RegisterLuaClass ( state );
	data.RegisterLuaClass ( state );
	this->mClassTable = state.GetStrongRef ( -1 );

	lua_setglobal ( state, data.TypeName ());

	lua_settop ( state, top );
}

//----------------------------------------------------------------//
void MOAILuaClass::InitLuaSingletonClass ( MOAILuaObject& data, MOAILuaState& state ) {

	int top = lua_gettop ( state );

	state.PushPtrUserData ( &data );

	lua_newtable ( state );
	this->RegisterLuaClass ( state );
	data.RegisterLuaClass ( state );
	this->mClassTable = state.GetStrongRef ( -1 );
	
	lua_pushvalue ( state, -1 );
	lua_setfield ( state, -2, "__index" );

	lua_pushnil ( state );
	lua_setfield ( state, -2, "__newindex" );

	lua_setmetatable ( state, -2 );

	lua_setglobal ( state, data.TypeName ());

	// set up the instance table so we can use lua retain/release
	lua_newtable ( state );
	data.mInstanceTable.SetStrongRef ( state, -1 );
	lua_pop ( state, 1 );

	lua_settop ( state, top );
}

//----------------------------------------------------------------//
bool MOAILuaClass::IsSingleton () {

	return ( this->GetSingleton () != 0 );
}

//----------------------------------------------------------------//
void MOAILuaClass::PushInterfaceTable ( MOAILuaState& state ) {

	state.Push ( this->mInterfaceTable );
}

//----------------------------------------------------------------//
MOAILuaClass::MOAILuaClass () {
}

//----------------------------------------------------------------//
MOAILuaClass::~MOAILuaClass () {
}

