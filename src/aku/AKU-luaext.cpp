// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include <aku/AKU-luaext.h>
#include <moaicore/moaicore.h>

extern "C" {
	#include <lcrypto.h>
	#include <luasocket.h>
	#include <mime.h>
	
	extern int luaopen_crypto			( lua_State *L );
	extern int luaopen_luacurl			( lua_State *L );
	extern int luaopen_luasql_sqlite3	( lua_State *L );
}

//================================================================//
// AKU-fmod
//================================================================//

//----------------------------------------------------------------//
void AKUExtLoadLuacrypto () {

	lua_State* state = AKUGetLuaState ();
	luaopen_crypto ( state );
}

//----------------------------------------------------------------//
void AKUExtLoadLuacurl () {

	lua_State* state = AKUGetLuaState ();
	luaopen_luacurl ( state );
}

//----------------------------------------------------------------//
void AKUExtLoadLuasocket () {

	lua_State* state = AKUGetLuaState ();
	//luaopen_socket_core ( state );
	
	luaL_Reg regTable [] = {
		{ "luaopen_socket_core",	luaopen_socket_core },
		{ "luaopen_mime_core",		luaopen_mime_core },
		{ NULL, NULL }
	};
	
	luaL_register ( state, "socketwrap", regTable );
}

//----------------------------------------------------------------//
void AKUExtLoadLuasql () {

	lua_State* state = AKUGetLuaState ();
	luaopen_luasql_sqlite3 ( state );
}
