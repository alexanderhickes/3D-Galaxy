#include <OpenGLFramework/Components/RenderComponent/SingleColourMesh_Renderable.h>
#include <OpenGLFramework\3DUI_Utils\3DUI_Utils.h>

using namespace OpenGLFramework;

 SingleColourMesh_Renderable::SingleColourMesh_Renderable(const int numVertex, const GLfloat vertex_buffer_data[]) :numVertex(numVertex){
	//0. Copy data to our local buffer (in main memory~CPU)
	g_vertex_buffer_data=new GLfloat[3*numVertex];
	memcpy(g_vertex_buffer_data,vertex_buffer_data,3*numVertex*sizeof(GLfloat));
	this->bb = ThreeDUI_Utils::createAABoundingBox(numVertex, (GLfloat*)&(vertex_buffer_data[0]));
}

bool  SingleColourMesh_Renderable::loadResourcesToMainMemory(){
	//Data was provided in the constructor. We do not need to load anything from a file.
	return true;
}

bool  SingleColourMesh_Renderable::allocateOpenGLResources(){
	//0. Create and compile our GLSL program from the shaders
	programID = ShaderManager::instance().LoadShaders("OpenGLFramework/shaders/ClipSpaceVertexShader.vertexshader", "OpenGLFramework/shaders/SingleColourFragmentShader.fragmentshader");
	//1. Get a handle for the input attribute in our shader
	vertexPosition_clipspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	//2. Create a buffer in the GPU and load it with our data (it is still in main memory)
	glGenBuffers(1, &vertexbuffer);
	this->setVertices(numVertex, g_vertex_buffer_data);
	//3. All ready to go in the GPU :)
	return true;

}
bool  SingleColourMesh_Renderable::render(glm::mat4 P, glm::mat4 V){
	//0. Use the default behaviour in the base class (do not render if not enabled)
	if(!OpenGL_Renderable::render(P, V))return false;
	
	//1. Tell OpenGL to use our shader
	glUseProgram(programID);
	
	//2. We do not use MVP, this object works in clipspace coords...
	// 2.1. Enable our 1st attribute: 
	glEnableVertexAttribArray(vertexPosition_clipspaceID);
	//2.2. Connect our attribute to the buffer of vertices (this will feed the data in the buffer to the shader's input attribute vertexPosition_clipspaceID)
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		vertexPosition_clipspaceID,  // The attribute we want to configure
		3,                            // size
		GL_FLOAT,                     // type
		GL_FALSE,                     // normalized?
		0,                            // stride
		(void*)0                      // array buffer offset
	);
	//3. Draw the mesh !
	glDisable(GL_CULL_FACE);							//Play with this. It enables/disables back-face culling
	glPointSize(5);										//If you are rendering points, this sets its size (in pixels)
	glDrawArrays(getRenderPrimitive(), 0, numVertex);	//Actually render the stuff
	glEnable(GL_CULL_FACE);								//Let's leave this on, it is generally a good idea
	
	//4. Let's disable our attribute now, we would not want someone to use it by accident.
	glDisableVertexAttribArray(vertexPosition_clipspaceID);
	return true;
}

bool  SingleColourMesh_Renderable::unallocateAllResources(){
	// Cleanup what we allocated in the GPU: VBO and shader (the attribute handler vertexPosition_clipspaceID is part of the shader, will be deleted with it)
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteProgram(programID);
	return true;


}

void  SingleColourMesh_Renderable::setVertices(const int numVertex, const GLfloat vertex_buffer_data[]){
	//The number of vertices should remain constant
	memcpy(g_vertex_buffer_data,vertex_buffer_data,3*numVertex*sizeof(GLfloat));
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, 3*numVertex*sizeof(GLfloat), g_vertex_buffer_data, GL_DYNAMIC_DRAW);	

}