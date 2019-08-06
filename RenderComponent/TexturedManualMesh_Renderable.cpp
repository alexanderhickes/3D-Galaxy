#include <OpenGLFramework\Components\RenderComponent\TexturedManualMesh_Renderable.h>
#include <OpenGLFramework\3DUI_Utils\3DUI_Utils.h>

using namespace OpenGLFramework;

TexturedManualMesh_Renderable::TexturedManualMesh_Renderable( const int numVertex, const GLfloat vertex_buffer_data[], const GLfloat uv_buffer_data[], std::string textureName):numVertex(numVertex){
	g_vertex_buffer_data=new GLfloat[3*numVertex];
	memcpy(g_vertex_buffer_data,vertex_buffer_data,3*numVertex*sizeof(GLfloat));	
	
	g_uv_buffer_data=new GLfloat[2*numVertex];
	memcpy(g_uv_buffer_data,uv_buffer_data,2*numVertex*sizeof(GLfloat));

	this->textureName=textureName;

	this->bb = ThreeDUI_Utils::createAABoundingBox(numVertex, (GLfloat*)&(vertex_buffer_data[0]));
}

TexturedManualMesh_Renderable::TexturedManualMesh_Renderable(const int numVertex, const GLfloat vertex_buffer_data[], const GLfloat uv_buffer_data[], GLuint Texture) :numVertex(numVertex){

	g_vertex_buffer_data=new GLfloat[3*numVertex];
	memcpy(g_vertex_buffer_data,vertex_buffer_data,3*numVertex*sizeof(GLfloat));

	g_uv_buffer_data=new GLfloat[2*numVertex];
	memcpy(g_uv_buffer_data,uv_buffer_data,2*numVertex*sizeof(GLfloat));
	
	this->textureName="";//The user provided a handler to the texture strainghtahead. Maybe it is a RTT or maybe the user has some external way of loading/creating the textures...
	this->Texture=Texture;
}
	

bool TexturedManualMesh_Renderable::loadResourcesToMainMemory(){
	return true;
}

bool TexturedManualMesh_Renderable::allocateOpenGLResources(){
	/* Load the texture. These methods actually load into main memory, but they also allocate
	   the texture in the graphics card already, returning a handler for the texture.
	   We support two formats, one for BMP and ne for DDS.	*/
	if(		textureName.substr(textureName.find_last_of(".") + 1) == "bmp" 
		  ||textureName.substr(textureName.find_last_of(".") + 1) == "BMP")
				Texture = loadBMP_custom(textureName.c_str());  
	else if(textureName.substr(textureName.find_last_of(".") + 1) == "dds" 
		  ||textureName.substr(textureName.find_last_of(".") + 1) == "DDS")
				Texture = loadDDS(textureName.c_str());
	else if(textureName=="")
		;//The user provided the texture himself/herself. No need to do anything more
	else //The user did give a name, but it does not have the correct extension...
		return false;

	// Create and compile our GLSL program from the shaders
	programID = ShaderManager::instance().LoadShaders("OpenGLFramework/shaders/MVPVertexShader.vertexshader", "OpenGLFramework/shaders/TextureFragmentShader.fragmentshader");
	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	// Get a handle for our buffers
	vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	vertexUVID = glGetAttribLocation(programID, "vertexUV");
	// Get a handle for our "myTextureSampler" uniform
	TextureID  = glGetUniformLocation(programID, "myTextureSampler");
	//Load raw data in OpenGL buffers...
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, 3*numVertex*sizeof(GLfloat), g_vertex_buffer_data, GL_STATIC_DRAW);
		
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, 2*numVertex*sizeof(GLfloat), g_uv_buffer_data, GL_STATIC_DRAW);
	
	return true;

}
bool TexturedManualMesh_Renderable::render(glm::mat4 P, glm::mat4 V){
	if(!OpenGL_Renderable::render(P, V))return false;
	// Use our shader
		glUseProgram(programID);
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glm::mat4 MVP        = P * V  * getOwner()->getFromObjectToWorldCoordinates();;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID, 0);

		// 1rst attribute buffer : vertices
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

		// 2nd attribute buffer : UVs
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

		// Draw the triangles !
		glDrawArrays(getRenderPrimitive(), 0, numVertex);
		glDisableVertexAttribArray(vertexPosition_modelspaceID);
		glDisableVertexAttribArray(vertexUVID);

		
	return true;

}

bool TexturedManualMesh_Renderable::unallocateAllResources(){
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &TextureID);
	return true;


}
