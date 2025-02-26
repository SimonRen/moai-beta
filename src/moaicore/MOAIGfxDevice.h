// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIGFXDEVICE_H
#define	MOAIGFXDEVICE_H

#include <moaicore/MOAIBlendMode.h>
#include <moaicore/MOAIColor.h>
#include <moaicore/MOAIEventSource.h>
#include <moaicore/MOAILua.h>

class MOAIFrameBuffer;
class MOAIGfxResource;
class MOAIShader;
class MOAITexture;
class MOAIVertexFormat;
class MOAIViewport;

//================================================================//
// MOAIGfxDevice
//================================================================//
/**	@name	MOAIGfxDevice
	@text	Interface to the graphics singleton.
*/
class MOAIGfxDevice :
	public MOAIGlobalClass < MOAIGfxDevice, MOAIGlobalEventSource > {
public:
	
	enum {
		VTX_STAGE_MODEL,
		VTX_STAGE_WORLD,
		VTX_STAGE_VIEW,
		VTX_STAGE_PROJ,
	};
	
	enum {
		UV_STAGE_MODEL,
		UV_STAGE_TEXTURE,
	};
	
	enum {
		VTX_WORLD_TRANSFORM,
		VTX_VIEW_TRANSFORM,
		VTX_PROJ_TRANSFORM,
		TOTAL_VTX_TRANSFORMS,
	};
	
	enum {
		EVENT_RESIZE,
		TOTAL_EVENTS,
	};
	
private:
	
	static const u32 DEFAULT_BUFFER_SIZE	= 0x8000;
	
	const MOAIVertexFormat*	mVertexFormat;
	
	void*			mBuffer;
	u32				mSize;
	u32				mTop;
	u32				mPrimTop;
	
	GLenum			mPrimType;
	u32				mPrimSize;
	u32				mPrimCount;
	u32				mMaxPrims;
	
	MOAITexture*	mTexture;
	MOAIShader*		mShader;
	
	USMatrix4x4		mUVTransform;
	USMatrix4x4		mVertexTransforms [ TOTAL_VTX_TRANSFORMS ];
	
	USColorVec		mPenColor;
	u32				mPackedColor;
	float			mPenWidth;
	float			mPointSize;
	
	MOAIBlendMode	mBlendMode;
	bool			mBlendEnabled;
	
	USRect			mScissorRect;
	USRect			mViewRect;
	
	u32				mWidth;
	u32				mHeight;

	u32				mUVMtxInput;
	u32				mUVMtxOutput;

	u32				mVertexMtxInput;
	u32				mVertexMtxOutput;

	bool			mCpuVertexTransform;
	USMatrix4x4		mCpuVertexTransformMtx; // composition of matrices to be applied via CPU
	
	bool			mCpuUVTransform;
	
	bool			mHasContext;
	bool			mIsOpenGLES;
	u32				mMajorVersion;
	u32				mMinorVersion;
	bool			mIsProgrammable;
	
	bool			mIsFramebufferSupported;
	GLuint			mDefaultFrameBuffer;
	
	size_t			mTextureMemoryUsage;

	typedef USLeanList < MOAIGfxResource* >::Iterator ResourceIt;
	USLeanList < MOAIGfxResource* > mResources;

	GLbitfield		mClearFlags;
	u32				mClearColor;
	MOAIColor*		mClearColorNode;

	//----------------------------------------------------------------//
	static int				_isProgrammable			( lua_State* L );
	static int				_setClearColor			( lua_State* L );
	static int				_setClearDepth			( lua_State* L );
	static int				_setPenColor			( lua_State* L );
	static int				_setPenWidth			( lua_State* L );
	static int				_setPointSize			( lua_State* L );

	//----------------------------------------------------------------//
	void					Clear					();
	void					DrawPrims				();
	void					GpuLoadMatrix			( const USMatrix4x4& mtx ) const;
	void					GpuMultMatrix			( const USMatrix4x4& mtx ) const;
	void					InsertGfxResource		( MOAIGfxResource& resource );
	void					RemoveGfxResource		( MOAIGfxResource& resource );
	void					ReportTextureAlloc		( cc8* name, size_t size );
	void					ReportTextureFree		( cc8* name, size_t size );
	void					UpdateCpuVertexMtx		();
	void					UpdateGpuVertexMtx		();
	void					UpdateUVMtx				();
	

public:
	
	friend class MOAIGfxResource;
	friend class MOAITexture;
	
	DECL_LUA_SINGLETON ( MOAIGfxDevice )
	
	GET ( size_t, TextureMemoryUsage, mTextureMemoryUsage )
	GET ( bool, HasContext, mHasContext )
	
	GET_BOOL ( IsOpenGLES, mIsOpenGLES )
	GET_BOOL ( IsProgrammable, mIsProgrammable )
	GET_BOOL ( IsFramebufferSupported, mIsFramebufferSupported )
	
	//----------------------------------------------------------------//
	void					BeginDrawing			();
	void					BeginLayer				();
	void					BeginPrim				();
	void					BeginPrim				( u32 primType );
	void					ClearColorBuffer		( u32 color );
	
	void					ClearErrors				();
	void					DetectContext			();
	void					DrawPrims				( const MOAIVertexFormat& format, GLenum primType, void* buffer, u32 size ); 
	void					EndPrim					();
	void					Flush					();
	
	cc8*					GetErrorString			( int error ) const;
	
	u32						GetHeight				() const;
	
	USMatrix4x4				GetModelToWndMtx		() const;
	USMatrix4x4				GetModelToWorldMtx		() const;
	
	USColorVec				GetPenColor				() const;
	USRect					GetRect					() const;
	USMatrix4x4				GetUVTransform			() const;
	USMatrix4x4				GetVertexTransform		( u32 id ) const;
	
	USMatrix4x4				GetViewProjMtx			() const;
	USQuad					GetViewQuad				() const;
	USRect					GetViewRect				() const;
	
	u32						GetWidth				() const;

	USMatrix4x4				GetWorldToModelMtx		() const;
	USMatrix4x4				GetWorldToWndMtx		( float xScale = 1.0f, float yScale = 1.0f ) const;
	USMatrix4x4				GetWndToModelMtx		() const;
	USMatrix4x4				GetWndToWorldMtx		() const;
	
	u32						LogErrors				();
	
							MOAIGfxDevice			();
							~MOAIGfxDevice			();
	
	void					RegisterLuaClass		( MOAILuaState& state );
	void					ReleaseResources		();
	void					RenewResources			();
	
	void					Reserve					( u32 size );
	void					ResetState				();
	
	void					SetBlendMode			();
	void					SetBlendMode			( const MOAIBlendMode& blendMode );
	void					SetBlendMode			( int srcFactor, int dstFactor );
	
	void					SetClearColor			( MOAIColor* color );
	void					SetDefaultFrameBuffer	( GLuint frameBuffer );
	void					SetFrameBuffer			( MOAITexture* texture );
	void					SetPenColor				( u32 color );
	void					SetPenColor				( const USColorVec& colorVec );
	void					SetPenColor				( float r, float g, float b, float a );
	void					SetPenWidth				( float penWidth );
	void					SetPointSize			( float pointSize );
	void					SetPrimType				( u32 primType );
	void					SetScissorRect			();
	void					SetScissorRect			( const USRect& rect );
	void					SetScreenSpace			( MOAIViewport& viewport );
	void					SetShader				( MOAIShader* shader = 0 );
	void					SetShaderPreset			( u32 preset );
	void					SetSize					( u32 width, u32 height );
	bool					SetTexture				( MOAITexture* texture = 0 );
	
	void					SetUVMtxMode			( u32 input, u32 output );
	void					SetUVTransform			();
	void					SetUVTransform			( const USAffine2D& transform );
	void					SetUVTransform			( const USMatrix4x4& transform );
	
	void					SetVertexFormat			();
	void					SetVertexFormat			( const MOAIVertexFormat& format );
	void					SetVertexMtxMode		( u32 input, u32 output );
	void					SetVertexPreset			( u32 preset );
	void					SetVertexTransform		( u32 id );
	void					SetVertexTransform		( u32 id, const USAffine2D& transform );
	void					SetVertexTransform		( u32 id, const USMatrix4x4& transform );
	
	void					SetViewport				();
	void					SetViewport				( const USRect& viewport );
	
	void					SoftReleaseResources	( u32 age );
	
	void					WriteQuad				( USVec2D* vtx, USVec2D* uv );
	
	//----------------------------------------------------------------//
	template < typename TYPE >
	inline void Write ( const TYPE& type ) {
		
		*( TYPE* )(( size_t )this->mBuffer + this->mTop ) = type;
		this->mTop += sizeof ( TYPE );
	}
	
	//----------------------------------------------------------------//
	inline void WriteColor ( float r, float g, float b, float a ) {
		UNUSED ( r );
		UNUSED ( g );
		UNUSED ( b );
		UNUSED ( a );
	}
	
	//----------------------------------------------------------------//
	inline void WritePenColor4b () {
		
		this->Write < u32 >( this->mPackedColor );
	}
	
	//----------------------------------------------------------------//
	inline void WritePenColor4f () {
		
		this->Write < USColorVec >( this->mPenColor );
	}
	
	//----------------------------------------------------------------//
	inline void WriteUV ( float u, float v ) {
	
		USVec2D uv;
		uv.mX = u;
		uv.mY = v;
	
		if ( this->mCpuUVTransform ) {
			this->mUVTransform.Transform ( uv );
		}
		this->Write ( uv );
	}
	
	//----------------------------------------------------------------//
	inline void WriteUV ( USVec2D uv ) {
	
		if ( this->mCpuUVTransform ) {
			this->mUVTransform.Transform ( uv );
		}
		this->Write ( uv );
	}
	
	//----------------------------------------------------------------//
	inline void WriteVtx ( float x, float y ) {
		
		USVec2D vtx;
		vtx.mX = x;
		vtx.mY = y;
		
		if ( this->mCpuVertexTransform ) {
			this->mCpuVertexTransformMtx.Transform ( vtx );	
		}
		this->Write ( vtx );
	}
	
	//----------------------------------------------------------------//
	inline void WriteVtx ( USVec2D vtx ) {
		
		if ( this->mCpuVertexTransform ) {
			this->mCpuVertexTransformMtx.Transform ( vtx );	
		}
		this->Write ( vtx );
	}
};

#endif
