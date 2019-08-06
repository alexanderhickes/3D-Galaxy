#include <OpenGLFramework\Components\RenderComponent\PhongShadingOBJMesh_Renderable.h>
#include <OpenGLFramework/common/objloader.hpp>
#include <OpenGLFramework\3DUI_Utils\3DUI_Utils.h>

using namespace OpenGLFramework;

bool PhongShadingOBJMesh_Renderable::loadResourcesToMainMemory(){

	// Read our .obj file into our raw data buffers
	bool res; 
	if(model!="")
		res= loadOBJ(model.c_str(), vertices, uvs, normals);
	this->bb = ThreeDUI_Utils::createAABoundingBox(vertices);
	return true;
}

bool PhongShadingOBJMesh_Renderable::allocateOpenGLResources(){
	/* Load the texture. These methods actually load into main memory, but they also allocate
	   the texture in the graphics card already, returning a handler for the texture.
	   We support two formats, one for BMP and ne for DDS.	*/
	if(		textureName.substr(textureName.find_last_of(".") + 1) == "bmp" 
		  ||textureName.substr(textureName.find_last_of(".") + 1) == "BMP")
				Texture = loadBMP_custom(textureName.c_str());  
	else if(textureName.substr(textureName.find_last_of(".") + 1) == "dds" 
		  ||textureName.substr(textureName.find_last_of(".") + 1) == "DDS")
				Texture = loadDDS(textureName.c_str());
	else if(		textureName.substr(textureName.find_last_of(".") + 1) == "jpg" 
		  ||textureName.substr(textureName.find_last_of(".") + 1) == "JPG")
				Texture = loadJPEG(textureName.c_str());  else if(textureName=="")
		;//The user provided the texture himself/herself. No need to do anything more
	else //The user did give a name, but it does not have the correct extension...
		return false;

	// Create and compile our GLSL program from the shaders
	programID = ShaderManager::instance().LoadShaders("OpenGLFramework/shaders/PointLightShading.vertexshader", "OpenGLFramework/shaders/PointLightShading.fragmentshader");
	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");			//MVP matrix (uniform)
	ViewMatrixID = glGetUniformLocation(programID, "V");		//View matrix(uniform)
	ModelMatrixID = glGetUniformLocation(programID, "M");		//Model matrix(uniform)
	// Get a handle for our buffers
	vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");	//Array of vertices
	vertexUVID = glGetAttribLocation(programID, "vertexUV");									//Array of UV coords
	vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");		//Array of normals
	lightID = glGetUniformLocation(programID, "LightPosition_worldspace");						//Location of light (uniform) 
	lightColorID = glGetUniformLocation(programID, "LightColor");	
	lightPowerID=glGetUniformLocation(programID, "LightPower");
	ShininessID=glGetUniformLocation(programID, "Shininess");;
	Ka_ID = glGetUniformLocation(programID, "Ka");
	Kd_ID = glGetUniformLocation(programID, "Kd");
	Ks_ID = glGetUniformLocation(programID, "Ks");
	TextureID  = glGetUniformLocation(programID, "myTextureSampler");							//Texture to use (uniform)
	// Load object data into OpenGL buffers (VBO)
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	return true;
}

bool PhongShadingOBJMesh_Renderable::render(glm::mat4 P, glm::mat4 V){
	if(!OpenGL_Renderable::render(P, V))return false;
	// Use our shader
		glUseProgram(programID);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glm::mat4 MVP = P * V * getOwner()->getFromObjectToWorldCoordinates();
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);										
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &(getOwner()->getFromObjectToWorldCoordinates())[0][0]);////We need to put our vertices in world coordinates (the light is in world coordinates)
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &V[0][0]);
		glUniform3f(lightID, lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(lightColorID, lightColor.x, lightColor.y, lightColor.z);
		glUniform1f(lightPowerID,lightPower);
		glUniform1f(ShininessID,Ns);
		glUniform3f(Ka_ID, Ka[0],Ka[1],Ka[2]);
		glUniform3f(Kd_ID, Kd[0],Kd[1],Kd[2]);
		glUniform3f(Ks_ID, Ks[0],Ks[1],Ks[2]);
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

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(vertexNormal_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			vertexNormal_modelspaceID,    // The attribute we want to configure
			3,                            // size
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
		);
		
		// Draw the triangles !
		glDrawArrays(getRenderPrimitive(), 0, vertices.size()); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(vertexPosition_modelspaceID);
		glDisableVertexAttribArray(vertexUVID);	
		glDisableVertexAttribArray(vertexNormal_modelspaceID);
	return true;
}

bool PhongShadingOBJMesh_Renderable::unallocateAllResources(){
	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &TextureID);
	return true;
}
