// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"
#include <moaicore/MOAIColor.h>
#include <moaicore/MOAIEaseDriver.h>
#include <moaicore/MOAIGfxDevice.h>
#include <moaicore/MOAILogMessages.h>
#include <moaicore/MOAIShader.h>
#include <moaicore/MOAITransformBase.h>

//================================================================//
// MOAIShaderUniform
//================================================================//

//----------------------------------------------------------------//
void MOAIShaderUniform::Bind () {

	if ( this->mIsDirty ) {
		
		this->mIsDirty = false;

		switch ( this->mType ) {
			
			case UNIFORM_INT:
				glUniform1i ( this->mAddr, this->mInt );
				break;
				
			case UNIFORM_FLOAT:
				glUniform1f ( this->mAddr, this->mFloat );
				break;
				
			case UNIFORM_COLOR:
				glUniform4fv ( this->mAddr, 1, this->mBuffer );
				break;
			
			case UNIFORM_VIEW_PROJ:
			case UNIFORM_WORLD:
			case UNIFORM_WORLD_VIEW_PROJ:
			case UNIFORM_TRANSFORM:
				glUniformMatrix4fv ( this->mAddr, 1, false, this->mBuffer );
				break;
		}
	}
}

//----------------------------------------------------------------//
void MOAIShaderUniform::BindPipelineTransforms ( const USMatrix4x4& world, const USMatrix4x4& view, const USMatrix4x4& proj ) {

	switch ( this->mType ) {
		
		case UNIFORM_VIEW_PROJ: {
			
			USMatrix4x4 mtx = view;
			mtx.Append ( proj );
			this->SetValue ( mtx );
			this->Bind ();
			break;
		}
		case UNIFORM_WORLD: {
			
			this->SetValue ( world );
			this->Bind ();
			break;
		}
		case UNIFORM_WORLD_VIEW_PROJ: {
			
			USMatrix4x4 mtx = world;
			mtx.Append ( view );
			mtx.Append ( proj );
			this->SetValue ( mtx );
			this->Bind ();
			break;
		}
	}
}

//----------------------------------------------------------------//
void MOAIShaderUniform::Clear () {

	this->mType = MOAIShaderUniform::UNIFORM_NONE;
	this->mBuffer.Clear ();
}

//----------------------------------------------------------------//
MOAIShaderUniform::MOAIShaderUniform () :
	mAddr ( 0 ),
	mType ( UNIFORM_NONE ),
	mIsDirty ( false ) {
}

//----------------------------------------------------------------//
MOAIShaderUniform::~MOAIShaderUniform () {
}

//----------------------------------------------------------------//
void MOAIShaderUniform::SetBuffer ( void* buffer, size_t size ) {

	if ( !this->mIsDirty ) {
		this->mIsDirty = ( memcmp ( this->mBuffer, buffer, size ) != 0 );
	}
	
	if ( this->mIsDirty ) {
		memcpy ( this->mBuffer, buffer, size );
	}
}

//----------------------------------------------------------------//
void MOAIShaderUniform::SetType ( u32 type ) {

	this->mBuffer.Clear ();
	
	this->mType = type;
	
	switch ( type ) {
		
		case UNIFORM_COLOR: {
			this->mBuffer.Init ( 4 );
			
			USColorVec color;
			color.Set ( 1.0f, 1.0f, 1.0f, 1.0f );
			this->SetValue ( color );
			break;
		}
		case UNIFORM_VIEW_PROJ:
		case UNIFORM_WORLD:
		case UNIFORM_WORLD_VIEW_PROJ:
		case UNIFORM_TRANSFORM: {
			this->mBuffer.Init ( 16 );
			
			USAffine2D mtx;
			mtx.Ident ();
			this->SetValue ( mtx );
			break;
		}
	};
	
	this->mIsDirty = true;
}

//----------------------------------------------------------------//
void MOAIShaderUniform::SetValue ( float value ) {

	if ( this->mFloat != value ) {
		this->mFloat = value;
		this->mIsDirty = true;
	}
}

//----------------------------------------------------------------//
void MOAIShaderUniform::SetValue ( int value ) {

	if ( this->mInt != value ) {
		this->mInt = value;
		this->mIsDirty = true;
	}
}

//----------------------------------------------------------------//
void MOAIShaderUniform::SetValue ( const MOAIAttrOp& attrOp ) {

	switch ( this->mType ) {
	
		case UNIFORM_COLOR: {
			USColorVec color;
			attrOp.GetValue < USColorVec >( color );
			this->SetValue ( color );
			break;
		}	
		case UNIFORM_FLOAT: {
			this->SetValue (( float )attrOp.GetValue ());
			break;
		}
		case UNIFORM_INT: {
			this->SetValue (( int )attrOp.GetValue ());
			break;
		}
		case UNIFORM_TRANSFORM: {
			USAffine2D* affine = attrOp.GetValue < USAffine2D >();
			if ( affine ) {
				this->SetValue ( *affine );
			}
			break;
		}
	}
}

//----------------------------------------------------------------//
void MOAIShaderUniform::SetValue ( const USColorVec& value ) {
	
	float m [ 4 ];
	
	m [ 0 ]		= value.mR;
	m [ 1 ]		= value.mG;
	m [ 2 ]		= value.mB;
	m [ 3 ]		= value.mA;
	
	this->SetBuffer ( m, sizeof ( m ));
}

//----------------------------------------------------------------//
void MOAIShaderUniform::SetValue ( const USAffine2D& value ) {
	
	float m [ 16 ];
	
	m [ 0 ]		= value.m [ AffineElem2D::C0_R0 ];
	m [ 1 ]		= value.m [ AffineElem2D::C0_R1 ];
	m [ 2 ]		= 0.0f;
	m [ 3 ]		= 0.0f;
	
	m [ 4 ]		= value.m [ AffineElem2D::C1_R0 ];
	m [ 5 ]		= value.m [ AffineElem2D::C1_R1 ];
	m [ 6 ]		= 0.0f;
	m [ 7 ]		= 0.0f;
	
	m [ 8 ]		= 0.0f;
	m [ 9 ]		= 0.0f;
	m [ 10 ]	= 1.0f;
	m [ 11 ]	= 0.0f;
	
	m [ 12 ]	= value.m [ AffineElem2D::C2_R0 ];
	m [ 13 ]	= value.m [ AffineElem2D::C2_R1 ];
	m [ 14 ]	= 0.0f;
	m [ 15 ]	= 1.0f;
	
	this->SetBuffer ( m, sizeof ( m ));
}

//----------------------------------------------------------------//
void MOAIShaderUniform::SetValue ( const USMatrix4x4& value ) {
	
	float m [ 16 ];
	
	m [ 0 ]		= value.m [ USMatrix4x4::C0_R0 ];
	m [ 1 ]		= value.m [ USMatrix4x4::C1_R0 ];
	m [ 2 ]		= value.m [ USMatrix4x4::C2_R0 ];
	m [ 3 ]		= value.m [ USMatrix4x4::C3_R0 ];
	
	m [ 4 ]		= value.m [ USMatrix4x4::C0_R1 ];
	m [ 5 ]		= value.m [ USMatrix4x4::C1_R1 ];
	m [ 6 ]		= value.m [ USMatrix4x4::C2_R1 ];
	m [ 7 ]		= value.m [ USMatrix4x4::C3_R1 ];
	
	m [ 8 ]		= value.m [ USMatrix4x4::C0_R2 ];
	m [ 9 ]		= value.m [ USMatrix4x4::C1_R2 ];
	m [ 10 ]	= value.m [ USMatrix4x4::C2_R2 ];
	m [ 11 ]	= value.m [ USMatrix4x4::C3_R2 ];
	
	m [ 12 ]	= value.m [ USMatrix4x4::C0_R3 ];
	m [ 13 ]	= value.m [ USMatrix4x4::C1_R3 ];
	m [ 14 ]	= value.m [ USMatrix4x4::C2_R3 ];
	m [ 15 ]	= value.m [ USMatrix4x4::C3_R3 ];
	
	this->SetBuffer ( m, sizeof ( m ));
}

//================================================================//
// local
//================================================================//

//----------------------------------------------------------------//
/**	@name	clearUniform
	@text	Clears a uniform mapping.

	@in		MOAIShader self
	@in		number idx
	@out	nil
*/
int MOAIShader::_clearUniform ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIShader, "UN" )
	
	u32 idx = state.GetValue < u32 >( 2, 1 ) - 1;
	
	self->ClearUniform ( idx );
	
	return 0;
}

//----------------------------------------------------------------//
/**	@name	declareUniform
	@text	Declares a uniform mapping.

	@in		MOAIShader self
	@in		number idx
	@in		string name
	@opt	number type		One of MOAIShader.UNIFORM_FLOAT, MOAIShader.UNIFORM_TRANSFORM,
							MOAIShader.UNIFORM_VIEW_PROJ, MOAIShader.UNIFORM_WORLD, MOAIShader.UNIFORM_WORLD_VIEW_PROJ
	@out	nil
*/
int MOAIShader::_declareUniform ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIShader, "UNS" )
	
	u32 idx				= state.GetValue < u32 >( 2, 1 ) - 1;
	STLString name		= state.GetValue < cc8* >( 3, "" );
	u32  type			= state.GetValue < u32 >( 4, MOAIShaderUniform::UNIFORM_NONE );
	
	self->DeclareUniform ( idx, name, type );

	return 0;
}

//----------------------------------------------------------------//
/**	@name	load
	@text	Load a shader program.

	@in		MOAIShader self
	@in		string vertexShaderSource
	@in		string fragmentShaderSource
	@out	nil
*/
int MOAIShader::_load ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIShader, "USS" )
	
	cc8* vtxSource	= state.GetValue < cc8* >( 2, 0 );
	cc8* frgSource	= state.GetValue < cc8* >( 3, 0 );
	
	self->SetSource ( vtxSource, frgSource );
	
	return 0;
}

//----------------------------------------------------------------//
/**	@name	reserveUniforms
	@text	Reserve shader uniforms.

	@in		MOAIShader self
	@opt	number nUniforms	Default value is 0.
	@out	nil
*/
int MOAIShader::_reserveUniforms ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIShader, "U" )
	
	u32 nUniforms = state.GetValue < u32 >( 2, 0 );
	self->ReserveUniforms ( nUniforms );
	
	return 0;
}

//----------------------------------------------------------------//
/**	@name	setVertexAttribute
	@text	Names a shader vertex attribute.

	@in		MOAIShader self
	@in		number index	Default value is 1.
	@in		string name		Name of attribute.
	@out	nil
*/
int MOAIShader::_setVertexAttribute ( lua_State* L ) {
	MOAI_LUA_SETUP ( MOAIShader, "UNS" )
	
	GLuint idx				= state.GetValue < GLuint >( 2, 1 ) - 1;
	STLString attribute		= state.GetValue < cc8* >( 3, "" );
	
	self->SetVertexAttribute ( idx, attribute );

	return 0;
}

//================================================================//
// MOAIShader
//================================================================//

//----------------------------------------------------------------//
bool MOAIShader::ApplyAttrOp ( u32 attrID, MOAIAttrOp& attrOp, u32 op ) {

	attrID = ( attrID & MOAINode::ATTR_ID_MASK ) - 1;

	if ( attrID >= this->mUniforms.Size ()) return false;

	if ( op == MOAIAttrOp::CHECK ) {
		attrOp.SetValid ( true, MOAINode::ATTR_WRITE );
		return true;
	}
	
	if ( op == MOAIAttrOp::SET ) {
		
		this->mUniforms [ attrID ].SetValue ( attrOp );
		return true;
	}
	return false;
}

//----------------------------------------------------------------//
void MOAIShader::BindUniforms () {

	for ( u32 i = 0; i < this->mUniforms.Size (); ++i ) {
		this->mUniforms [ i ].Bind ();
	}
}

//----------------------------------------------------------------//
void MOAIShader::ClearUniform ( u32 idx ) {

	if ( idx < this->mUniforms.Size ()) {
		this->mUniforms [ idx ].Clear ();
	}
}

//----------------------------------------------------------------//
void MOAIShader::ClearUniforms () {

	this->mUniforms.Clear ();
}

//----------------------------------------------------------------//
GLuint MOAIShader::CompileShader ( GLuint type, cc8* source ) {

	MOAIGfxDevice& gfxDevice = MOAIGfxDevice::Get ();

	GLuint shader = glCreateShader ( type );
	cc8* sources [ 2 ];
	
	sources [ 0 ] = gfxDevice.IsOpenGLES () ? OPENGL_ES_PREPROC : OPENGL_PREPROC;
	sources [ 1 ] = source;
	
	glShaderSource ( shader, 2, sources, NULL );
	glCompileShader ( shader );

	GLint status;
	glGetShaderiv ( shader, GL_COMPILE_STATUS, &status );
	
	if ( status == 0 ) {
		
		int logLength;
		glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &logLength );
 
		char* log = ( char* )malloc ( logLength );
 
		glGetShaderInfoLog ( shader, logLength, &logLength, log );
		MOAILog ( 0, MOAILogMessages::MOAIShader_ShaderInfoLog_S, log );
		
		free ( log );
	
		glDeleteShader ( shader );
		return 0;
	}

	return shader;
}

//----------------------------------------------------------------//
void MOAIShader::DeclareUniform ( u32 idx, cc8* name, u32 type ) {
	
	if ( idx < this->mUniforms.Size ()) {
		
		this->ClearUniform ( idx );
		
		MOAIShaderUniform& uniform = this->mUniforms [ idx ];
		uniform.mName = name;
		uniform.SetType ( type );
	}
}

//----------------------------------------------------------------//
bool MOAIShader::IsRenewable () {

	return true;
}

//----------------------------------------------------------------//
bool MOAIShader::IsValid () {

	return ( this->mProgram != 0 );
}

//----------------------------------------------------------------//
MOAIShader::MOAIShader () :
	mProgram ( 0 ),
	mVertexShader ( 0 ),
	mFragmentShader ( 0 ) {
	
	RTTI_BEGIN
		RTTI_EXTEND ( MOAINode )
		RTTI_EXTEND ( MOAIGfxResource )
	RTTI_END
}

//----------------------------------------------------------------//
MOAIShader::~MOAIShader () {

	this->Clear ();
}

//----------------------------------------------------------------//
void MOAIShader::OnBind () {

	// use shader program.
	glUseProgram ( this->mProgram );

	// reload the uniform values
	for ( u32 i = 0; i < this->mUniforms.Size (); ++i ) {
		this->mUniforms [ i ].Bind ();
	}
}

//----------------------------------------------------------------//
void MOAIShader::OnClear () {
	
	this->mVertexShaderSource.clear ();
	this->mFragmentShaderSource.clear ();
	
	this->mAttributeMap.clear ();
	this->ClearUniforms ();
}

//----------------------------------------------------------------//
void MOAIShader::OnLoad () {

	this->mVertexShader = this->CompileShader ( GL_VERTEX_SHADER, this->mVertexShaderSource );
	this->mFragmentShader = this->CompileShader ( GL_FRAGMENT_SHADER, this->mFragmentShaderSource );
	this->mProgram = glCreateProgram ();
	
	if ( !( this->mVertexShader && this->mFragmentShader && this->mProgram )) {
		this->Clear ();
		this->SetError ();
		return;
	}
    
	glAttachShader ( this->mProgram, this->mVertexShader );
	glAttachShader ( this->mProgram, this->mFragmentShader );
    
	// bind attribute locations.
	// this needs to be done prior to linking.
	AttributeMapIt attrMapIt = this->mAttributeMap.begin ();
	for ( ; attrMapIt != this->mAttributeMap.end (); ++attrMapIt ) {
		glBindAttribLocation ( this->mProgram, attrMapIt->first, attrMapIt->second.str ());
	}
    
    // link program.
	glLinkProgram ( this->mProgram );
	
	GLint status;
	glGetProgramiv ( this->mProgram, GL_LINK_STATUS, &status );
	
	if ( status == 0 ) {
		this->Clear ();
		this->SetError ();
		return;
	}
	
	// get the uniform locations and clear out the names (no longer needed)
	for ( u32 i = 0; i < this->mUniforms.Size (); ++i ) {
		MOAIShaderUniform& uniform = this->mUniforms [ i ];
		
		if ( uniform.mType != MOAIShaderUniform::UNIFORM_NONE ) {
			uniform.mAddr = glGetUniformLocation ( this->mProgram, uniform.mName );
			uniform.mName.clear ();
		}
	}

	glDeleteShader ( this->mVertexShader );
	this->mVertexShader = 0;
	
	glDeleteShader ( this->mFragmentShader );
	this->mFragmentShader = 0;
	
	this->mAttributeMap.clear ();
}

//----------------------------------------------------------------//
void MOAIShader::OnRenew () {

	// don't need to do anything here - vertex and fragment source should
	// already be cached
}

//----------------------------------------------------------------//
void MOAIShader::OnUnload () {

	if ( this->mVertexShader ) {
		glDeleteShader ( this->mVertexShader );
		this->mVertexShader = 0;
	}
	
	if ( this->mFragmentShader ) {
		glDeleteShader ( this->mFragmentShader );
		this->mFragmentShader = 0;
	}
	
	if ( this->mProgram ) {
		glDeleteProgram ( this->mProgram );
		this->mProgram = 0;
	}
}

//----------------------------------------------------------------//
void MOAIShader::RegisterLuaClass ( MOAILuaState& state ) {
	
	MOAINode::RegisterLuaClass ( state );
	MOAIGfxResource::RegisterLuaClass ( state );
	
	state.SetField ( -1, "UNIFORM_INT",					( u32 )MOAIShaderUniform::UNIFORM_INT );
	state.SetField ( -1, "UNIFORM_FLOAT",				( u32 )MOAIShaderUniform::UNIFORM_FLOAT );
	state.SetField ( -1, "UNIFORM_COLOR",				( u32 )MOAIShaderUniform::UNIFORM_COLOR );
	state.SetField ( -1, "UNIFORM_TRANSFORM",			( u32 )MOAIShaderUniform::UNIFORM_TRANSFORM );
	state.SetField ( -1, "UNIFORM_VIEW_PROJ",			( u32 )MOAIShaderUniform::UNIFORM_VIEW_PROJ );
	state.SetField ( -1, "UNIFORM_WORLD",				( u32 )MOAIShaderUniform::UNIFORM_WORLD );
	state.SetField ( -1, "UNIFORM_WORLD_VIEW_PROJ",		( u32 )MOAIShaderUniform::UNIFORM_WORLD_VIEW_PROJ );
}

//----------------------------------------------------------------//
void MOAIShader::RegisterLuaFuncs ( MOAILuaState& state ) {
	
	MOAINode::RegisterLuaFuncs ( state );
	MOAIGfxResource::RegisterLuaFuncs ( state );
	
	luaL_Reg regTable [] = {
		{ "clearUniform",				_clearUniform },
		{ "declareUniform",				_declareUniform },
		{ "load",						_load },
		{ "reserveUniforms",			_reserveUniforms },
		{ "setVertexAttribute",			_setVertexAttribute },
		{ NULL, NULL }
	};
	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAIShader::ReserveUniforms ( u32 nUniforms ) {

	this->ClearUniforms ();
	this->mUniforms.Init ( nUniforms );
}

//----------------------------------------------------------------//
void MOAIShader::SetSource ( cc8* vshSource, cc8* fshSource ) {

	if ( vshSource && fshSource ) {
		this->mVertexShaderSource = vshSource;
		this->mFragmentShaderSource = fshSource;
	}
}

//----------------------------------------------------------------//
void MOAIShader::SetVertexAttribute ( u32 idx, cc8* attribute ) {

	if ( attribute ) {
		this->mAttributeMap [ idx ] = attribute;
	}
}

//----------------------------------------------------------------//
void MOAIShader::UpdatePipelineTransforms ( const USMatrix4x4& world, const USMatrix4x4& view, const USMatrix4x4& proj ) {

	// reload the uniform values
	for ( u32 i = 0; i < this->mUniforms.Size (); ++i ) {
		this->mUniforms [ i ].BindPipelineTransforms ( world, view, proj );
	}
}

//----------------------------------------------------------------//
bool MOAIShader::Validate () {

    GLint logLength;
    
    glValidateProgram ( this->mProgram );
    glGetProgramiv ( this->mProgram, GL_INFO_LOG_LENGTH, &logLength );
	
    if ( logLength > 0 ) {
        char* log = ( char* )malloc ( logLength );
        glGetProgramInfoLog ( this->mProgram, logLength, &logLength, log );
        MOAILog ( 0, MOAILogMessages::MOAIShader_ShaderInfoLog_S, log );
        free ( log );
    }
    
	GLint status;
    glGetProgramiv ( this->mProgram, GL_VALIDATE_STATUS, &status );
    if ( status == 0 ) {
		return false;
	}
	return true;
}
