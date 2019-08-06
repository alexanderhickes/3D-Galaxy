/**********************************************************************
NAME: DirectionalLightOBJMesh_Renderable.h
DESCRIPTION: Loads an OBJ model from a file (vertices, UVs, normals) and renders it with a texture applied. 
Diffuse ilumination, ambient light and reflective lights are considered, using a directional light as a reference. 
The texture can be either a file (.bmp or .dds) or externally generated (other file readers, a Render to Texture, etc.).
The object is described in local coordinates, but its model matrix can be used to move it within the world. 

SHADERS: 
	- DirectionalLightShading.vertexshader: This is a slightly more complicated shader that we are used to. We compute the clip_space
	coordinates of our vertex and the UV coordinates as usual. We also put both the object normal and the light direction vectors 
	in world coordinates. We will need this in the fragment shader, to compute the brightness of the light. 
	- DirectionalLightShading.fragmentshader: This fragment shader reads the colour information from the object's texture using its
	UV coordinates. Then it computes the angle between the normal and the light and computes the contribution of the light.
NEXT OBJECT TO CHECK: PhongShadingOBJMesh_Renderable (and that is the last one... good job!!)
**************************************************************************************************************/

#ifndef _DIRECTIONAL_LIGHT_OBJ_MESH_RENDERABLE
#define _DIRECTIONAL_LIGHT_OBJ_MESH_RENDERABLE
#include <OpenGLFramework\Components\RenderComponent\OpenGL_Renderable.h>
#include <vector>

namespace OpenGLFramework {
	class DirectionalLightOBJMesh_Renderable : public OpenGL_Renderable {
		//Config parameters
		std::string textureName, model;
		//std::string vertexShader, fragmentShader; //This is fixed, as the arguments (see "vertexPosition_modelspace" "myTextureSampler" init) are specific for the shaders, changing programs makes no sense...
		//Raw data (as read from a file, etc...)
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		glm::vec3 lightDir;
		glm::vec3 lightColor;
		//float lightPower;	//Power does not affect directional light (Excercise: Why? How is it different from PointLight?)

		//OpenGL handlers
		GLuint programID;
		GLuint MatrixID, ModelMatrixID; //We also need the model matrix, not only the final MVP
		GLuint vertexPosition_modelspaceID;
		GLuint vertexUVID;
		GLuint vertexNormal_modelspaceID;
		GLuint lightID;							//Direction of the light.
		GLuint lightColorID;					//Color of the light
		GLuint TextureID;
		//Actual OpenGL data structures
		GLuint Texture;
		GLuint vertexbuffer;
		GLuint uvbuffer;
		GLuint normalbuffer;

	public:
		//Own methods
		DirectionalLightOBJMesh_Renderable(std::string model, std::string texture, glm::vec3 lightDir = glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f))
			: TextureID(-1), textureName(texture), model(model), lightDir(lightDir), lightColor(lightColor)
		{
			;
		}
		inline void configureDirectionalLight(glm::vec3 dir, glm::vec3 color) { lightDir = dir; lightColor = color; }		
		//Methods inherited from base class OpenGL_Renderable
		virtual bool loadResourcesToMainMemory();
		virtual bool allocateOpenGLResources();
		virtual bool render(glm::mat4 P, glm::mat4 V);
		virtual bool unallocateAllResources();
	};
};
#endif