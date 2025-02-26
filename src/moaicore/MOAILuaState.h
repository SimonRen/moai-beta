// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef MOAILUASTATE_H
#define MOAILUASTATE_H

class USCipher;
class MOAILuaRef;
class MOAILuaObject;
class USStreamFormatter;

#define LUA_SETUP(type,str) \
	MOAILuaState state ( L );	\
	if ( !state.CheckParams ( 1, str )) return 0; \
	type* self = state.GetLuaObject < type >( 1 ); \
	if ( !self ) return 0;

#define LUA_SETUP_STATIC(str) \
	MOAILuaState state ( L );	\
	if ( !state.CheckParams ( 1, str )) return 0;

//================================================================//
// MOAILuaState
//================================================================//
class MOAILuaState {
private:

	lua_State*	mState;

	//----------------------------------------------------------------//
	bool			Decode					( int idx, USCipher& cipher );
	bool			Encode					( int idx, USCipher& cipher );
	bool			Transform				( int idx, USStreamFormatter& formatter );

public:

	friend class MOAILuaStateHandle;

	//----------------------------------------------------------------//
	int				AbsIndex				( int idx );
	bool			Base64Decode			( int idx );
	bool			Base64Encode			( int idx );
	bool			CheckParams				( int idx, cc8* format ); // "BCFLNSTU"
	void			CopyToTop				( int idx );
	int				DebugCall				( int nArgs, int nResults );
	bool			Deflate					( int idx, int level, int windowBits );
	void			GetField				( int idx, cc8* name );
	void			GetField				( int idx, int key );
	STLString		GetField				( int idx, cc8* key, cc8* value );
	STLString		GetField				( int idx, int key, cc8* value );
	bool			GetFieldWithType		( int idx, cc8* name, int type );
	bool			GetFieldWithType		( int idx, int key, int type );
	void*			GetPtrUserData			( int idx );
	STLString		GetStackTrace			( int level );
	MOAILuaRef		GetStrongRef			( int idx );
	int				GetTop					();
	void*			GetUserData				( int idx, void* value );
	void*			GetUserData				( int idx, cc8* name, void* value );
	STLString		GetValue				( int idx, cc8* value );
	MOAILuaRef		GetWeakRef				( int idx );
	bool			HasField				( int idx, cc8* name );
	bool			HasField				( int idx, int key );
	bool			HasField				( int idx, cc8* name, int type );
	bool			HasField				( int idx, int name, int type );
	bool			Inflate					( int idx, int windowBits );
	bool			IsNil					();
	bool			IsNil					( int idx );
	bool			IsTableOrUserdata		( int idx );
	bool			IsType					( int idx, int type );
	bool			IsType					( int idx, cc8* name, int type );
	void			LoadLibs				();
	void			MoveToTop				( int idx );
	void			Pop						( int n );
	bool			PrepMemberFunc			( int idx, cc8* name );
	bool			PrintErrors				( FILE* file, int status );
	void			PrintStackTrace			( FILE* file, int level );
	void			Push					( bool value );
	void			Push					( cc8* value );
	void			Push					( double value );
	void			Push					( float value );
	void			Push					( int value );
	void			Push					( u16 value );
	void			Push					( u32 value );
	void			Push					( u64 value );
	void			Push					( uintptr value );
	void			Push					( lua_CFunction value );
	void			Push					( MOAILuaObject* luaObject );
	void			Push					( MOAILuaRef& ref );
	void			PushPtrUserData			( void* ptr );
	int				PushTableItr			( int idx );
	void			RegisterModule			( cc8* name, lua_CFunction loader, bool autoLoad );
	int				RelIndex				( int idx );
	void			SetPath					( cc8* path );
	void			SetTop					( int top );
	bool			TableItrNext			( int itr );
					MOAILuaState				();
					MOAILuaState				( lua_State* state );
	virtual			~MOAILuaState				();
	int				YieldThread				( int nResults );
	
	//----------------------------------------------------------------//
	inline lua_State* operator -> () const {
		return mState;
	};

	//----------------------------------------------------------------//
	inline lua_State& operator * () const {
		return *mState;
	};

	//----------------------------------------------------------------//
	inline operator lua_State* () {
		return mState;
	};

	//----------------------------------------------------------------//
	inline operator bool () {
		return ( this->mState != 0 );
	}
	
	//----------------------------------------------------------------//
	template < typename TYPE > TYPE						GetField			( int idx, int key, TYPE value );
	template < typename TYPE > TYPE						GetField			( int idx, cc8* key, TYPE value );
	template < typename TYPE > TYPE*					GetLuaObject		( int idx );
	template < typename TYPE > TYPE*					GetLuaObject		( int idx, cc8* name );
	template < typename TYPE > USMetaRect < TYPE >		GetRect				( int idx );
	template < typename TYPE > TYPE						GetValue			( int idx, TYPE value );
	template < typename TYPE > USMetaVec2D < TYPE >		GetVec2D			( int idx );
	template < typename TYPE > USMetaVec3D < TYPE >		GetVec3D			( int idx );
	template < typename TYPE > TYPE						PopValue			( TYPE value );
	template < typename TYPE > void						ReadArray			( int size, TYPE* values, TYPE value );
	template < typename TYPE > void						SetField			( int idx, cc8* key, TYPE value );
	template < typename TYPE > void						SetFieldByIndex		( int idx, int key, TYPE value );
	template < typename TYPE > void						WriteArray			( int size, TYPE* values );
};

//----------------------------------------------------------------//
template <> bool		MOAILuaState::GetValue < bool >		( int idx, bool value );
template <> cc8*		MOAILuaState::GetValue < cc8* >		( int idx, cc8* value );
template <> double		MOAILuaState::GetValue < double >		( int idx, double value );
template <> float		MOAILuaState::GetValue < float >		( int idx, float value );
template <> int			MOAILuaState::GetValue < int >		( int idx, int value );
template <> u8			MOAILuaState::GetValue < u8 >			( int idx, u8 value );
template <> u16			MOAILuaState::GetValue < u16 >		( int idx, u16 value );
template <> u32			MOAILuaState::GetValue < u32 >		( int idx, u32 value );
template <> u64			MOAILuaState::GetValue < u64 >		( int idx, u64 value );
template <> uintptr		MOAILuaState::GetValue < uintptr >	( int idx, uintptr value );

#endif
