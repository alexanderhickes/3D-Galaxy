/**********************************************************************
NAME: PhongShadingOBJMesh.h
DESCRIPTION: Loads an OBJ model from a file (vertices, UVs, normals) and renders it with a texture applied. 
Diffuse ilumination, ambient light and reflective lights are considered, using a point light as a reference. 
The texture can be either a file (.bmp or .dds) or externally generated (other file readers, a Render to Texture, etc.).
The object is described in local coordinates, but its model matrix can be used to move it within the world. 
The lighting and the shaders are very explained in detail in the lectures, 
but you can check http://www.opengl-tutorial.org/beginners-tutorials/tutorial-8-basic-shading/, in case you missed this.
Shaders can be found in: shaders/PointLightShading.vertexshader and shaders/PointLightShading.fragmentshader 

NEXT OBJECT TO CHECK: NONE. You made it to the end. I hope you had fun and learnt quite a bit. 
	Now you are ready to create your own shaders and extend this framework! 
**************************************************************************************************************/

#ifndef _PHONG_OBJ_MESH_RENDERABLE
#define _PHONG_OBJ_MESH_RENDERABLE
#include <OpenGLFramework\Components\RenderComponent\OpenGL_Renderable.h>
#include <vector>

namespace OpenGLFramework {
	class PhongShadingOBJMesh_Renderable : public OpenGL_Renderable {
		//Config parameters
		std::string textureName, model;
		//std::string vertexShader, fragmentShader; //This is fixed, as the arguments (see "vertexPosition_modelspace" "myTextureSampler" init) are specific for the shaders, changing programs makes no sense...
		//Raw data (as read from a file, etc...)
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		glm::vec3 lightPos;
		glm::vec3 lightColor;
		float Ka[3], Kd[3], Ks[3], Ns, lightPower;
		//OpenGL handlers
		GLuint programID;
		GLuint MatrixID, ViewMatrixID, ModelMatrixID; //We also need the intermediate matrixes, not only the final MVP
		GLuint vertexPosition_modelspaceID;
		GLuint vertexUVID;
		GLuint vertexNormal_modelspaceID;
		GLuint lightID;							//Position of the light.
		GLuint lightColorID, lightPowerID, ShininessID;
		GLuint Ka_ID, Kd_ID, Ks_ID;				//Phong components of the material.
		GLuint TextureID;
		//Actual OpenGL data structures
		GLuint Texture;
		GLuint vertexbuffer;
		GLuint uvbuffer;
		GLuint normalbuffer;

	public:
		//Own methods
		PhongShadingOBJMesh_Renderable(std::string model, std::string texture, glm::vec3 lightPos = glm::vec3(4, 4, 4), float lightPower = 75.0f)
			: TextureID(-1), textureName(texture), model(model), lightPos(lightPos), lightPower(lightPower)
		{
			;
		}

		PhongShadingOBJMesh_Renderable(std::vector<glm::vec3>vertices, std::vector<glm::vec2> uvs, std::vector<glm::vec3> normals, std::string texture, glm::vec3 lightPos = glm::vec3(4, 4, 4), float lightPower = 75.0f)
			: vertices(vertices), uvs(uvs), normals(normals)
			, TextureID(-1), textureName(texture), model(""), lightPos(lightPos), lightPower(lightPower)
		{
			;
		}
		PhongShadingOBJMesh_Renderable(const int numVertex, const GLfloat vertex_buffer_data[], const GLfloat uv_buffer_data[], const GLfloat normal_buffer_data[], std::string texture, glm::vec3 lightPos = glm::vec3(4, 4, 4), float lightPower = 75.0f)
			: TextureID(-1), textureName(texture), model(""), lightPos(lightPos), lightPower(lightPower)
		{
			for (int i = 0; i < numVertex; i++) {
				vertices.push_back(glm::vec3(vertex_buffer_data[3 * i], vertex_buffer_data[3 * i + 1], vertex_buffer_data[3 * i + 2]));
				uvs.push_back(glm::vec2(uv_buffer_data[2 * i], uv_buffer_data[2 * i + 1]));
				normals.push_back(glm::vec3(normal_buffer_data[3 * i], normal_buffer_data[3 * i + 1], normal_buffer_data[3 * i + 2]));
			}
		}
		void setMaterial(float _Ka[3], float _Kd[3], float _Ks[3], float _Ns) {
			Ka[0] = _Ka[0]; Ka[1] = _Ka[1]; Ka[2] = _Ka[2];
			Kd[0] = _Kd[0]; Kd[1] = _Kd[1]; Kd[2] = _Kd[2];
			Ks[0] = _Ks[0]; Ks[1] = _Ks[1]; Ks[2] = _Ks[2];
			Ns = _Ns;
		}

		inline void setLight(glm::vec3 pos, glm::vec3 color, float intensity) { lightPos = pos; lightColor = color; lightPower = intensity; }
		//Methods inherited from base class OpenGL_Renderable
		virtual bool loadResourcesToMainMemory();
		virtual bool allocateOpenGLResources();
		virtual bool render(glm::mat4 P, glm::mat4 V);
		virtual bool unallocateAllResources();
	};
};
#endif