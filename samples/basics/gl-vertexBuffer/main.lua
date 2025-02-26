----------------------------------------------------------------
-- Copyright (c) 2010-2011 Zipline Games, Inc. 
-- All Rights Reserved. 
-- http://getmoai.com
----------------------------------------------------------------

MOAISim.openWindow ( "test", 320, 480 )

viewport = MOAIViewport.new ()
viewport:setSize ( 320, 480 )
viewport:setScale ( 320, 480 )

layer = MOAILayer2D.new ()
layer:setViewport ( viewport )
MOAISim.pushRenderPass ( layer )

vertexFormat = MOAIVertexFormat.new ()

vertexFormat:declareCoord ( 1, MOAIVertexFormat.GL_FLOAT, 2 )
vertexFormat:declareColor ( 2, MOAIVertexFormat.GL_UNSIGNED_BYTE )
vertexFormat:declareUV ( 3, MOAIVertexFormat.GL_FLOAT, 2 )

vbo = MOAIVertexBuffer.new ()
vbo:setFormat ( vertexFormat )
vbo:reserveVerts ( 4 )
vbo:setPrimType ( MOAIVertexBuffer.GL_TRIANGLE_FAN )

vbo:writeFloat ( -64, -64 )
vbo:writeColor32 ( 1, 0, 0 )
vbo:writeFloat ( 0, 1 )

vbo:writeFloat ( 64, -64 )
vbo:writeColor32 ( 1, 1, 0 )
vbo:writeFloat ( 1, 1 )

vbo:writeFloat ( 64, 64 )
vbo:writeColor32 ( 0, 1, 0 )
vbo:writeFloat ( 1, 0 )

vbo:writeFloat ( -64, 64 )
vbo:writeColor32 ( 0, 0, 1 )
vbo:writeFloat ( 0, 0 )

vbo:bless ()

mesh = MOAIMesh.new ()
mesh:setTexture ( "cathead.png" )
mesh:setVertexBuffer ( vbo )

if MOAIGfxDevice.isProgrammable () then

	file = assert ( io.open ( 'shader.vsh', mode ))
	vsh = file:read ( '*all' )
	file:close ()

	file = assert ( io.open ( 'shader.fsh', mode ))
	fsh = file:read ( '*all' )
	file:close ()

	shader = MOAIShader.new ()

	shader:reserveUniforms ( 1 )
	shader:declareUniform ( 1, 'transform', MOAIShader.UNIFORM_TRANSFORM )
	
	shader:setVertexAttribute ( 1, 'position' )
	shader:setVertexAttribute ( 2, 'color' )
	shader:setVertexAttribute ( 3, 'uv' )

	shader:load ( vsh, fsh )
	
	mesh:setShader ( shader )
end

prop = MOAIProp2D.new ()
prop:setDeck ( mesh )
prop:moveRot ( 360, 1.5 )
layer:insertProp ( prop )

