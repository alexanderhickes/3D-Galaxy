/**********************************************************************
NAME: TexturedOBJMesh_Renderable (check UnitPolygonTextured_Renderable first)
DESCRIPTION: Loads an OBJ model from a file (vertices, UVs, normals) and renders it with a texture applied. 
No lighting is considered yet (flat texture only).
The texture can be either a file (.bmp or .dds) or externally generated (other file readers, a Render to Texture, etc.).
The object's vertices is described in local coordinates, but its model matrix can be used to move it within the world. 

SHADERS (Same than UnitPolygonTexturedh_Renderable): 
	- MVPVertexShader.vertexshader: Simple vertex shader that uses world coordinates. It descibes several attributes:
		a) vertexPosition_modelspace: As usual, this corresponds to the array of vertex.
		b) vertexUV: Texture mapping coordinates for each vertex (array of uv coordinates)
		c) uniform MVP: this contains the model-view-projection matrix that transforms coordinates in the object's local space
		to clip_space (camera) coordinates. The flag "uniform" means that all the vertex use the same value. We will retrieve a 
		handle for this attribute and update it every frame (we will build the current MVP matrix according to the current 
		position of the camera (V), its frustum (P) and the object location(M)) 
		d) It adds an output variable UV, which is transfered to the fragment shader. Varying denotes that, for the fragments 
		interpolated between frames (the pixles inside of the triangle), the UV value will be interpolated from the UVs of the 
		verted, just as in barycentric interpolation.

	- TexturedFragentShader.fragmentshader: Simple fragment shader that renders each fragment from a texture and UV coordinates.
		a) UV: This is the interpolated UV coordinates of this fragment. That is, the point inside the texture we will get the fragment colour from
		b) myTextureSample: This is the texture we read colours from. It is a uniform, as all fragments get their colour from the same
		texture file. 
NEXT OBJECT TO CHECK: DirectionalLightOBJMesh_Renderable 

**************************************************************************************************************/

#ifndef _TEXTURED_OBJ_MESH_RENDERABLE
#define _TEXTURED_OBJ_MESH_RENDERABLE
#include <OpenGLFramework\Components\RenderComponent\OpenGL_Renderable.h>
#include <vector>

namespace OpenGLFramework {
	class TexturedOBJMesh_Renderable : public OpenGL_Renderable {
		//Config parameters
		std::string textureFileName, modelFileName;
		//std::string vertexShader, fragmentShader; //This is fixed, as the arguments (see "vertexPosition_modelspace" "myTextureSampler" init) are specific for the shaders, changing programs makes no sense...
		//Raw data (as read from a file, etc...)
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals; // Won't be used at the moment.
		//OpenGL handlers
		GLuint programID;
		GLuint MatrixID;
		GLuint vertexPosition_modelspaceID;
		GLuint vertexUVID;
		GLuint Texture;
		GLuint TextureID;
		GLuint vertexbuffer;
		GLuint uvbuffer;



	public:
		//Own methods:
		TexturedOBJMesh_Renderable(std::string model, std::string texture)
			: TextureID(-1), textureFileName(texture), modelFileName(model)
		{
			;
		}
		//Methods inherited from base class OpenGL_Renderable
		virtual bool loadResourcesToMainMemory();
		virtual bool allocateOpenGLResources();
		virtual bool render(glm::mat4 P, glm::mat4 V);
		virtual bool unallocateAllResources();
	};
};
#endif