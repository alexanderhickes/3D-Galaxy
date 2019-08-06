#include <OpenGLFramework\Components\RenderComponent\TexturedOBJMesh_Renderable.h>
#include <OpenGLFramework/common/objloader.hpp>
#include <OpenGLFramework\3DUI_Utils\3DUI_Utils.h>

using namespace OpenGLFramework;

bool TexturedOBJMesh_Renderable::loadResourcesToMainMemory(){

	// Read our .obj file into our raw data buffers
	bool res = loadOBJ(modelFileName.c_str(), vertices, uvs, normals);
	this->bb = ThreeDUI_Utils::createAABoundingBox(vertices);
	//The method we use to load textures from a file, already allocates it into the GPU, so we will do that in allocateOpenGLResources
	return true;
}

bool TexturedOBJMesh_Renderable::allocateOpenGLResources(){
	/* Load the texture. These methods actually load into main memory, but they also allocate
	the texture in the graphics card already, returning a handler for the texture.
	We support two formats, one for BMP and ne for DDS.	*/
	if (textureFileName.substr(textureFileName.find_last_of(".") + 1) == "bmp"
		|| textureFileName.substr(textureFileName.find_last_of(".") + 1) == "BMP")
		Texture = loadBMP_custom(textureFileName.c_str());
	else if (textureFileName.substr(textureFileName.find_last_of(".") + 1) == "dds"
		|| textureFileName.substr(textureFileName.find_last_of(".") + 1) == "DDS")
		Texture = loadDDS(textureFileName.c_str());
	else if (textureFileName == "")
		;//The user provided the texture himself/herself. No need to do anything more
	else //The user did give a name, but it does not have the correct extension...
		return false;

	// Create and compile our GLSL program from the shaders
	programID = ShaderManager::instance().LoadShaders("OpenGLFramework/shaders/MVPVertexShader.vertexshader", "OpenGLFramework/shaders/TextureFragmentShader.fragmentshader");
	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	// Get a handler for our buffers
	vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	vertexUVID = glGetAttribLocation(programID, "vertexUV");
	// Get a handler for our "myTextureSampler" uniform
	TextureID  = glGetUniformLocation(programID, "myTextureSampler");
	// Load object data into OpenGL buffers (VBO for vertices and UVs)
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	return true;
}

bool TexturedOBJMesh_Renderable::render(glm::mat4 P, glm::mat4 V){
	if(!OpenGL_Renderable::render(P, V))return false;
	// Use our shader
		glUseProgram(programID);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glm::mat4 MVP        = P * V * getOwner()->getFromObjectToWorldCoordinates();
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
		glDrawArrays(getRenderPrimitive(), 0, vertices.size()); 

		glDisableVertexAttribArray(vertexPosition_modelspaceID);
		glDisableVertexAttribArray(vertexUVID);		
	return true;
}

bool TexturedOBJMesh_Renderable::unallocateAllResources(){
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &TextureID);
	return true;
}
