#include <OpenGLFramework/Components/RenderComponent/PerVertexColourMesh_Renderable.h>
#include <OpenGLFramework\3DUI_Utils\3DUI_Utils.h>

using namespace OpenGLFramework;

 PerVertexColourMesh_Renderable::PerVertexColourMesh_Renderable( const int numVertex, const GLfloat vertex_buffer_data[], const GLfloat colour_buffer_data[], bool allocateOwnBuffers):numVertex(numVertex), localBuffers(allocateOwnBuffers){
	if (allocateOwnBuffers)
	{
		//This is the most usual scenario. The object is defined and stores its own copy of the data
		//This makes it independent from the client, but updating becomes slower (data needs to be dumped from client to object buffers)
		g_vertex_buffer_data = new GLfloat[3 * numVertex];
		memcpy(g_vertex_buffer_data, vertex_buffer_data, 3 * numVertex*sizeof(GLfloat));
		g_colour_buffer_data = new GLfloat[3 * numVertex];
		memcpy(g_colour_buffer_data, colour_buffer_data, 3 * numVertex*sizeof(GLfloat));
	}
	else{
		//This is usefull when the data is dynamically changed very often (e.g. a point cloud for a Kinect depth camera). 
		//We really do not want to be copying data every frame, so we just "share" the buffers with the client.
		//If the client releases that memory, we will be in serious trouble...
		g_vertex_buffer_data =(GLfloat*) &(vertex_buffer_data[0]);
		g_colour_buffer_data =(GLfloat*) &(colour_buffer_data[0]);
	}
	this->bb = ThreeDUI_Utils::createAABoundingBox(numVertex, (GLfloat*)&(vertex_buffer_data[0]));
}

bool  PerVertexColourMesh_Renderable::loadResourcesToMainMemory(){
	return true;
}

bool  PerVertexColourMesh_Renderable::allocateOpenGLResources(){
	// Create and compile our GLSL program from the shaders
	programID = ShaderManager::instance().LoadShaders("OpenGLFramework/shaders/MVP_PerVertexColor.vertexshader", "OpenGLFramework/shaders/PerVertexColorFragmentShader.fragmentshader");
	// Get a handle for our buffers
	vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	vertexColourID = glGetAttribLocation(programID, "vertexColor");
	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	//Load raw data in OpenGL buffers...
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, 3*numVertex*sizeof(GLfloat), g_vertex_buffer_data, GL_STATIC_DRAW);	

	glGenBuffers(1, &colourbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colourbuffer);
	glBufferData(GL_ARRAY_BUFFER, 3*numVertex*sizeof(GLfloat), g_colour_buffer_data, GL_STATIC_DRAW);	
	return true;

}
bool  PerVertexColourMesh_Renderable::render(glm::mat4 P, glm::mat4 V){
	//0. Use the default behaviour in the base class (do not render if not enabled)
	if (!OpenGL_Renderable::render(P, V))return false;

	//1. Tell OpenGL to use our shader
	glUseProgram(programID);

	//2. Configure our attributes:
	// 2.1. Configure our MVP matrix first, and set its value (it is a "uniform"-> same value for all vertices)
	glm::mat4 MVP        = P * V * getOwner()->getFromObjectToWorldCoordinates();
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	
	// 2.2. Enable and configure our 2nd attribute: vertices
	glEnableVertexAttribArray(vertexPosition_modelspaceID);
	// Connect our attribute to the buffer of vertices (this will feed the data in the buffer to the shader's input attribute vertexPosition_modelspaceID)
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		vertexPosition_modelspaceID,  // The attribute we want to configure
		3,                            // size
		GL_FLOAT,                     // type
		GL_FALSE,                     // normalized?
		0,                            // stride
		(void*)0                      // array buffer offset
	);
	// 2.3. Enable and configure our 3rd attribute: colors
	glEnableVertexAttribArray(vertexColourID);
	glBindBuffer(GL_ARRAY_BUFFER, colourbuffer);
	glVertexAttribPointer(
		vertexColourID,               // The attribute we want to configure
		3,                           // size
		GL_FLOAT,                    // type
		GL_FALSE,                    // normalized?
		0,                           // stride
		(void*)0                     // array buffer offset
	);
	
	// 3. Draw it!
	glDrawArrays(getRenderPrimitive(), 0, numVertex);

	//4. Let's disable our attributes now, we would not want someone to use them by accident.
	glDisableVertexAttribArray(vertexPosition_modelspaceID);
	glDisableVertexAttribArray(vertexColourID);
	return true;
}

bool  PerVertexColourMesh_Renderable::unallocateAllResources(){
	// Cleanup what we allocated in the GPU: VBOs and shader (the attribute handler vertexPosition_clipspaceID is part of the shader, will be deleted with it)
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colourbuffer);
	glDeleteProgram(programID);
	return true;
}


void  PerVertexColourMesh_Renderable::setVertices(const int numVertex, const GLfloat vertex_buffer_data[]){
	//The number of vertices should remain constant
	if (localBuffers)
		memcpy(g_vertex_buffer_data, vertex_buffer_data, 3 * numVertex*sizeof(GLfloat));
	else
		g_vertex_buffer_data = (GLfloat*)&(vertex_buffer_data[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, 3*numVertex*sizeof(GLfloat), g_vertex_buffer_data, GL_DYNAMIC_DRAW);	

}

void  PerVertexColourMesh_Renderable::setColours(const int numVertex, const GLfloat colour_buffer_data[]){
	//The number of vertices should remain constant
	if (localBuffers)
		memcpy(g_colour_buffer_data, colour_buffer_data, 3 * numVertex*sizeof(GLfloat));
	else
		g_colour_buffer_data = (GLfloat*)&(colour_buffer_data[0]);
	glBindBuffer(GL_ARRAY_BUFFER, colourbuffer);
	glBufferData(GL_ARRAY_BUFFER, 3 * numVertex*sizeof(GLfloat), g_colour_buffer_data, GL_DYNAMIC_DRAW);

}