#include <OpenGLFramework/Components/RenderComponent/UnitPolygonTextured_Renderable.h>

using namespace OpenGLFramework;

bool  UnitPolygonTextured_Renderable::loadResourcesToMainMemory(){return true;}

bool  UnitPolygonTextured_Renderable::allocateOpenGLResources(){
	// Create and compile our GLSL program from the shaders
	programID = ShaderManager::instance().LoadShaders("OpenGLFramework/shaders/MVPVertexShader.vertexshader", "OpenGLFramework/shaders/TextureFragmentShader.fragmentshader");
	
	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	
	// Get a handle for our buffers
	vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	vertexUVID = glGetAttribLocation(programID, "vertexUV");
	/* Load the texture. These methods actually load it into main memory, but they also allocate
	   the texture in the graphics card already, returning a handler for the texture.
	   We support two formats, one for BMP and one for DDS.	*/
	if(		textureFileName.substr(textureFileName.find_last_of(".") + 1) == "bmp" 
		  ||textureFileName.substr(textureFileName.find_last_of(".") + 1) == "BMP")
				Texture = loadBMP_custom(textureFileName.c_str());  
	else if(textureFileName.substr(textureFileName.find_last_of(".") + 1) == "dds" 
		  ||textureFileName.substr(textureFileName.find_last_of(".") + 1) == "DDS")
				Texture = loadDDS(textureFileName.c_str());
	else if(textureFileName=="")
		;//The user provided the texture himself/herself. No need to do anything more
	else //The user did give a name, but it does not have the correct extension...
		return false;//... we indicate we failed misserably

	// Get a handle for our "myTextureSampler" uniform
	TextureID  = glGetUniformLocation(programID, "myTextureSampler");
	
	// Our vertices for the unit plane. 
	//Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// Our unit polygon requires 2 triangles, so 6 vertices. 
	static const GLfloat g_vertex_buffer_data[] = { 
		-0.50f, 0.50f, 0.0f,
		-0.50f,-0.50f, 0.0f,
		 0.50f,-0.50f, 0.0f,
		
		 0.50f, 0.50f, 0.0f,
		-0.50f, 0.50f, 0.0f,
		 0.50f,-0.50f, 0.0f
	};

	// Two UV coordinatesfor each vertex.
	static const GLfloat g_uv_buffer_data[] = { 
		0,0,
		0,1,
		1,1,
		1,0,
		0,0,
		1,1
	};

	//Load this data in OpenGL buffers (position of the corners of the plane and UV coordinates of each vertex)
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
		
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);
	
	return true;

}
bool  UnitPolygonTextured_Renderable::render(glm::mat4 P, glm::mat4 V){
	//0. Use the default behaviour in the base class (do not render if not enabled)
	if(!OpenGL_Renderable::render(P, V))return false;
	
	//1. Tell OpenGL to use our shader
	glUseProgram(programID);

	//2. Configure our attributes:
	// 2.1. Configure our MVP matrix first, and set its value (it is a "uniform"-> same value for all vertices)
	glm::mat4 MVP = P * V * getOwner()->getFromObjectToWorldCoordinates();
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	//2.2. Bind our texture as Texture Unit 0 (our shader only uses one texture...)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture);
	// Set our "myTextureSampler" sampler to user Texture Unit 0. That is, set our handler to use the texture we allocated.
	glUniform1i(TextureID, 0);

	// 2.3. Configure our buffer of 3D vertices (and wire to attribute)
	glEnableVertexAttribArray(vertexPosition_modelspaceID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
			vertexPosition_modelspaceID,  // The attribute we want to configure
			3,                            // size
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
	);

	// 2.4. Configure our buffer of UVs (and wire to attribute)
	glEnableVertexAttribArray(vertexUVID);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
			vertexUVID,                   // The attribute we want to configure
			2,                            // size : U+V => 2
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
	);

	// 3. Draw it!
	glDisable(GL_CULL_FACE);			//Let's render both sides of the plane
	glDrawArrays(GL_TRIANGLES, 0, 2*3); // 2 triangles
	glEnable(GL_CULL_FACE);				//... but do not forget to restore afterwards.
	
	//4. Let's disable our attributes now, we would not want someone to use them by accident.
	glDisableVertexAttribArray(vertexPosition_modelspaceID);
	glDisableVertexAttribArray(vertexUVID);		
	return true;
}

bool  UnitPolygonTextured_Renderable::unallocateAllResources(){
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &TextureID);
	return true;
}
