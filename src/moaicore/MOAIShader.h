// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAISHADER_H
#define	MOAISHADER_H

#include <moaicore/MOAIGfxResource.h>
#include <moaicore/MOAINode.h>
#include <moaicore/MOAILua.h>

class MOAIColor;
class MOAITransformBase;
	
#define		OPENGL_PREPROC		"#define LOWP\n #define MEDP\n"
#define		OPENGL_ES_PREPROC	"#define LOWP lowp\n #define MEDP mediump\n"

//================================================================//
// MOAIShaderUniform
//================================================================//
class MOAIShaderUniform {
private:

	friend class MOAIShader;

	enum {
		UNIFORM_NONE,
		UNIFORM_COLOR,
		UNIFORM_FLOAT,
		UNIFORM_INT,
		UNIFORM_TRANSFORM,
		UNIFORM_VIEW_PROJ,
		UNIFORM_WORLD,
		UNIFORM_WORLD_VIEW_PROJ,
	};

	STLString mName;
	
	u32		mAddr;		// this is resolved when linking the shader
	u32		mType;
	bool	mIsDirty;

	USLeanArray < float > mBuffer;	
	
	union {
		float	mFloat;
		int		mInt;
	};

	//----------------------------------------------------------------//
	void		Bind						();
	void		BindPipelineTransforms		( const USMatrix4x4& world, const USMatrix4x4& view, const USMatrix4x4& proj );
	void		Clear						();
	void		SetBuffer					( void* buffer, size_t size );
	void		SetType						( u32 type );
	void		SetValue					( float value );
	void		SetValue					( int value);
	void		SetValue					( const MOAIAttrOp& attrOp );
	void		SetValue					( const USColorVec& value );
	void		SetValue					( const USAffine2D& value );
	void		SetValue					( const USMatrix4x4& value );

public:

	//----------------------------------------------------------------//
				MOAIShaderUniform			();
				~MOAIShaderUniform			();
};

//================================================================//
// MOAIShader
//================================================================//
/**	@name	MOAIShader
	@text	Programmable shader class.
*/
class MOAIShader :
	public virtual MOAINode,
	public MOAIGfxResource {
protected:
	
	STLString		mVertexShaderSource;
	STLString		mFragmentShaderSource;
	
	GLuint			mProgram;
	GLuint			mVertexShader;
	GLuint			mFragmentShader;
	
	typedef STLMap < GLuint, STLString >::iterator AttributeMapIt;
	STLMap < GLuint, STLString > mAttributeMap;
	
	USLeanArray < MOAIShaderUniform > mUniforms;
	
	//----------------------------------------------------------------//
	static int		_clearUniform			( lua_State* L );
	static int		_declareUniform			( lua_State* L );
	static int		_load					( lua_State* L );
	static int		_reserveUniforms		( lua_State* L );
	static int		_setVertexAttribute		( lua_State* L );
	
	//----------------------------------------------------------------//
	GLuint			CompileShader				( GLuint type,  cc8* source );
	bool			IsRenewable					();
	void			OnBind						();
	void			OnClear						();
	void			OnLoad						();
	void			OnRenew						();
	void			OnUnload					();
	void			UpdatePipelineTransforms	( const USMatrix4x4& world, const USMatrix4x4& view, const USMatrix4x4& proj );
	bool			Validate					();

public:
	
	DECL_LUA_FACTORY ( MOAIShader )
	
	friend class MOAIGfxDevice;
	
	//----------------------------------------------------------------//
	bool			ApplyAttrOp				( u32 attrID, MOAIAttrOp& attrOp, u32 op );
	void			BindUniforms			();
	void			ClearUniform			( u32 idx );
	void			ClearUniforms			();
	void			DeclareUniform			( u32 idx, cc8* name, u32 type );
	bool			IsValid					();
					MOAIShader				();
					~MOAIShader				();
	void			RegisterLuaClass		( MOAILuaState& state );
	void			RegisterLuaFuncs		( MOAILuaState& state );
	void			ReserveAttributes		( u32 nAttributes );
	void			ReserveUniforms			( u32 nUniforms );
	void			SetSource				( cc8* vshSource, cc8* fshSource );
	void			SetVertexAttribute		( u32 idx, cc8* attribute );
};

#endif
