
/**********************************************************************
NAME: PerVertexColourMesh_Renderable (check SingleColourMesh_Renderable first)
DESCRIPTION: Creates an object, with a different colour for each vertex. 
The 3D coordinates and vertex colours are provided in the constructor and are expresed in object local coordinates. 
As a big improvement, this object does render the object in world coordinates (uses MVP matrix) 
SHADERS: 
	- ClipSpacePerVertexColour.vertexshader: Simplest vertex shader that transform from object coordinates to 
	clip space coordinates (uses MVP). DIFFERENCES TO PREVIOUS (SingleColourMesh_Renderable): 
		i) Content not fixed in front of the camera;
		ii) It uses an aditional input attribute (the array of colours)
		iii) It adds an output variable fragmentColor, which is transfered to the fragment shader.
	- PerVertexColourFragentShader.fragmentshader: Simple fragment shader that renders each fragment according to its
	attribute fragmentColor. DIFFERENCE TO PREVIOUS: It does not use a fixed colour, it reads it from an attribute, which
	came from the vertex shader (actually, it is an interpolated value).
NEXT OBJECT TO CHECK: PerVertexColourMesh_Renderable
************************************************************************/


#ifndef _OPENGL_MANUAL_PERVERTEXCOLOUR
#define _OPENGL_MANUAL_PERVERTEXCOLOUR
#include <OpenGLFramework/Components/RenderComponent/OpenGL_Renderable.h>




namespace OpenGLFramework {
	class PerVertexColourMesh_Renderable : public OpenGL_Renderable {
		//Manual data (raw data provided by the user)
		int numVertex;
		GLfloat* g_vertex_buffer_data;
		GLfloat* g_colour_buffer_data;
		//OpenGL specific attributes.
		GLuint programID;
		GLuint MatrixID;
		//Attribute handlers
		GLuint vertexPosition_modelspaceID;
		GLuint vertexColourID;
		//Data handlers
		bool localBuffers;
		GLuint vertexbuffer;
		GLuint colourbuffer;

	public:
		//Own methods
		PerVertexColourMesh_Renderable(const int numVertex, const GLfloat vertex_buffer_data[], const GLfloat colour_buffer_data[], bool allocateOwnBuffers = true);
		void setVertices(const int numVertex, const GLfloat vertex_buffer_data[]);
		void setColours(const int numVertex, const GLfloat colour_buffer_data[]);
		//Methods inherited from base class OpenGL_Renderable
		virtual bool loadResourcesToMainMemory();
		virtual bool allocateOpenGLResources();
		virtual bool render(glm::mat4 P, glm::mat4 V);
		virtual bool unallocateAllResources();
	};
};
#endif
