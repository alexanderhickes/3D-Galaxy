/**********************************************************************
NAME: Unit Polygon Textured (check PerVertexColourMesh_Renderable first)
DESCRIPTION: Creates a unit polygon sitting on the XY plane with the normal pointing towards the +Z axis.
The plane has a texture applied (described in the constructor) which can be either a file (.bmp or .dds) or externally generated
(other file readers, a Render to Texture, etc.). 
The polygon is described in local object coordinates, and spans (-0.5, 0.5) units in X and (-0.5, 0.5) units in X. 
However, you can use the model matrix of the object we are attached to to:
	- move/rotate it within the world. 
	- change its size, by modifying the scaling of the object. 
This component contains data structures to contain the vertices and the uv coordinates, and the texture. 

SHADERS: 
	- MVPVertexShader.vertexshader: Simple vertex shader that uses world coordinates. It descibes several attributes:
		a) vertexPosition_modelspace: As usual, this corresponds to the array of vertex (3d local coords).
		b) vertexUV: Texture coordinates for each vertex (array of uv coordinates)
		c) uniform MVP: this contains the model-view-projection matrix that transforms coordinates in the object's local space
		to clip_space (camera) coordinates. The flag "uniform" means that all the vertex use the same value. We will retrieve a 
		handle for this attribute and update it every frame (we will build the current MVP matrix according to the current 
		position of the camera (V), its frustum (P) and the object location in the world(M)) 
		d) It adds an output variable UV, which is transfered to the fragment shader. Varying denotes that, for the fragments 
		interpolated between vertices (e.g. the pixles inside of the triangle), the UV value will be interpolated from the UVs of the 
		vertices, just as in barycentric interpolation.

	- TexturedFragentShader.fragmentshader: Simple fragment shader that renders each fragment from a texture and UV coordinates.
		a) UV: This is the interpolated UV coordinates of this fragment. That is, the point inside the texture we will get the fragment colour from
		b) myTextureSample: This is the texture we read colours from. It is a uniform, so all fragments get their colour from the same texture. 
NEXT OBJECT TO CHECK: TexturedOBJMesh_Renderable & TexturedManualMesh_Renderable (they are very similar)
************************************************************************/

#ifndef _OPENGL_UNIT_POLYGON_TEXTURED
#define _OPENGL_UNIT_POLYGON_TEXTURED
#include <OpenGLFramework/Components/RenderComponent/OpenGL_Renderable.h>

namespace OpenGLFramework {
	class UnitPolygonTextured_Renderable : public OpenGL_Renderable {
		//Name of the texture to apply to the polygon
		std::string textureFileName;

		//OpenGL Stuff
		GLuint programID; //Program handler containing the vertex shader and pixel shader
		//Handlers for shader's attributes:
		GLuint MatrixID;					//handler for the MVP matrix (this is a uniform --> same value used for all vertices)
		GLuint vertexPosition_modelspaceID; //handler for the 3D coordinates in our vertex shader
		GLuint vertexUVID;					//handler for the UV coordinates of each vertex (for our fragment shader).
		GLuint TextureID;					//handler for the texture to use (for the fragment shader)
		//Actual OpenGL data structures (data allocated in the graphics card memory which we can access for main memory)
		GLuint vertexbuffer;
		GLuint uvbuffer;
		GLuint Texture;
	public:
		//Own Methods
		UnitPolygonTextured_Renderable(std::string textureFileName = "uvtemplate.bmp") :textureFileName(textureFileName) { 
			//Set the sixe of the bounding box of our unit polygon
			bb.xmin=bb.ymin=-0.5f; 
			bb.xmax = bb.ymax = 0.5;
			bb.zmin = -0.01f; bb.zmax = 0.01f;	//It still needs a thickness 
		}
		UnitPolygonTextured_Renderable(GLuint Texture) :textureFileName(""), Texture(Texture) { ; }
		//Methods inherited from base class OpenGL_Renderable
		virtual bool loadResourcesToMainMemory();
		virtual bool allocateOpenGLResources();
		virtual bool render(glm::mat4 P, glm::mat4 V);
		virtual bool unallocateAllResources();
	};
};
#endif