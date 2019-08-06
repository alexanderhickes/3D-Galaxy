
/**********************************************************************
NAME: SingleColourMesh_Renderable
DESCRIPTION: Creates an object rendered using a single colour. The coordinates are described as
a list of triangles in the constructor and are expresed in clip space coordinates. 
Thus, the rendering ignores the MVP matrix and renders in clipspace coordinates. 
SHADERS: 
	- ClipSpaceVertexShader.vertexshader: Simple vertex shader that assumes coordinates are, already in 
	clip space coordinates
	- SingleColourFragentShader.fragmentshader: Simple fragment shader that renders all fragments red.
NEXT OBJECT TO CHECK: UnitPolygonTextured_Renderable
************************************************************************/


#ifndef _OPENGL_MANUAL_SINGLECOLOUR
#define _OPENGL_MANUAL_SINGLECOLOUR
#include <OpenGLFramework/Components/RenderComponent/OpenGL_Renderable.h>



namespace OpenGLFramework {
	class SingleColourMesh_Renderable : public OpenGL_Renderable {
		//Manual triangle mesh (raw data provided by the user). These are stored in the CPU --> we need to transfer them to the GPU
		int numVertex;
		GLfloat* g_vertex_buffer_data;

		//OpenGL specific attributes: All this referes to stuff in the GPU:
		GLuint programID;						//ID of the shader program to be used (it will be loaded in the GPU). This ID will allow us to tell the GPU to use this shader.
		//Attribute handlers
		GLuint vertexPosition_clipspaceID;		//ID of the input attribute of the shader. This will allow us to tell OpenGL to feed our vertices into this shader's input attribute
		//Data handlers
		GLuint vertexbuffer;					//Identifies our buffer of vertices in the GPU. This will allow us to copy vertices from CPU to GPU and to feed them to the shader's attribute (vertexPosition_clipspaceID)


	public:
		//Own methods
		SingleColourMesh_Renderable(const int numVertex, const GLfloat vertex_buffer_data[]);
		void setVertices(const int numVertex, const GLfloat vertex_buffer_data[]);
		//Methods inherited from base class OpenGL_Renderable
		virtual bool loadResourcesToMainMemory();
		virtual bool allocateOpenGLResources();
		virtual bool render(glm::mat4 P, glm::mat4 V);
		virtual bool unallocateAllResources();
	};
};
#endif