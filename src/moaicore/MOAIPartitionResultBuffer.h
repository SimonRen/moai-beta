// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIPARTITIONRESULTBUFFER_H
#define	MOAIPARTITIONRESULTBUFFER_H

class MOAIProp;

//================================================================//
// MOAIPartitionResultBuffer
//================================================================//
/**	@name	MOAIPartitionResultBuffer
	@text	Class for optimizing spatial queries against sets of primitives.
			Configure for performance; default behavior is a simple list.
*/
class MOAIPartitionResultBuffer {
private:

	friend class MOAIPartition;
	friend class MOAIPartitionCell;

	typedef USRadixKey32 < MOAIProp* > QueryEntry;

	static const u32 BLOCK_SIZE = 512;

	USLeanArray < QueryEntry >		mMainBuffer;
	USLeanArray < QueryEntry >		mSwapBuffer;
	QueryEntry*						mResults;
	u32								mTotalResults;

	//----------------------------------------------------------------//
	void			FinishQuery						();
	
public:

	enum SORT {
		SORT_NONE,
		SORT_PRIORITY_ASCENDING,
		SORT_PRIORITY_DESCENDING,
		SORT_X_ASCENDING,
		SORT_X_DESCENDING,
		SORT_Y_ASCENDING,
		SORT_Y_DESCENDING,
	};

	GET ( u32, TotalResults, mTotalResults )
	
	//----------------------------------------------------------------//
	void			Clear							();
					MOAIPartitionResultBuffer		();
					~MOAIPartitionResultBuffer		();
	MOAIProp*		PopResult						();
	void			PushResult						( MOAIProp& result );
	void			PushResultsList					( lua_State* L );
	void			Reset							();
	void			Sort							( u32 mode );
	void			Sort							( u32 mode, const USVec3D& scale );
	
	//----------------------------------------------------------------//
	inline MOAIProp* GetResult ( u32 idx ) {

		if ( this->mResults && ( idx < this->mTotalResults )) {
			return this->mResults [ idx ].mData;
		}
		return 0;
	}
	
	//----------------------------------------------------------------//
	inline MOAIProp* GetResultUnsafe ( u32 idx ) {
		
		return this->mResults [ idx ].mData;
	}
};

#endif
