// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef USDATA_H
#define USDATA_H

#include <uslsext/USMutex.h>

class USDataIOTask;

//================================================================//
// USData
//================================================================//
class USData {
private:

	USMutex				mMutex;
	USLeanArray < u8 >	mBytes;

	//----------------------------------------------------------------//
	bool			Decode					( USCipher& cipher );
	bool			Encode					( USCipher& cipher );
	bool			Transform				( USStreamFormatter& formatter );

public:

	//----------------------------------------------------------------//
	bool			Base64Decode		();
	bool			Base64Encode		();
	void			Clear				();
	bool			Deflate				( int level, int windowBits );
	bool			Inflate				( int windowBits );
	bool			Load				( cc8* filename );
	void			Load				( void* bytes, size_t size );
	void			Lock				( void** bytes, size_t* size );
	bool			Save				( cc8* filename, bool affirm_path = true );
	void			Unlock				();
					USData				();
	virtual			~USData				();
};

#endif
