/**********************************************************************
NAME: OpenGL_Renderable
DESCRIPTION: Base class that describes how to render some geometry, attached to a virtual object (a component). 
	This is the main place to start reading, if you want to understand how rendering works in OpenGL. 
	Reading through this file, you will learn the main stages involved in setting up and rendering content. 
	The next thing to do is to go to the subclasses of OpenGL_Renderable, to start seeing examples of 
	different types of content, and how to render it. The section to the bottom (NEXT OBJECT TO CHECK) should 
	help guiding your steps. 
	To trully understand how these subclasses work, you will need to look at both the C++ code (here in Visual Studio)
	and the GLSL shaders that these subclasses use (the code that will run in the GPU). 
	The file comments give you a brief introduction to how they work, but you should definitively open both C++ and GLSL 
	files, to understand the behaviour of the object.

NEXT OBJECT TO CHECK: SingleColourMesh_Renderable
***********************************************************************/

#ifndef _OPENGL_RENDERABLE
#define _OPENGL_RENDERABLE

#include "OpenGLFRameworkPrerequisites.h"// Include GLM
#include <OpenGLFramework/common/ShaderManager.hpp>
#include <OpenGLFramework/common/texture.hpp>
#include <OpenGLFramework\Components\IComponent.h>

namespace OpenGLFramework {
	class OpenGL_Renderable : public IComponent {
		GLuint renderPrimitive;					//How do we want to render (which primitive? GL_LINES, GL_TRIANGLES, GL_POINTS, etc...) 
	protected: 
		/**
			This is a bounding box (local to the object). Thus, it does not need to be recomputed each time we move the object (or parent nodes)
			Each Renderable must fill in this field, according to its geometry (e.g. when read from a file).
			If the renderable changes its geometry (e.g. changes local position of a vertex in its buffers), this should be updated.
		*/
		BoundingBox bb;
	public:
		//Functionality related to base class Component
		virtual std::string getComponentType() const { 
			//All components that iinherit from OpenGL_Renderable are labeled as "Renderable"
			return "Renderable"; 
		}

		//Own behaviour
		OpenGL_Renderable() :renderPrimitive(GL_TRIANGLES) { 
			bb.xmin = bb.ymin = bb.zmin = -1;
			bb.xmax = bb.ymax = bb.zmax = 1;
		}
		/**
			This is the first step of the initialization of any content we render in the GPU. 
			This method will load the data (from files, textures, etc...), do any initial processing (adapt format), compute normals (if not in the file) etc...
			At the end of the method, all information is ready in main memory to be used, but nothing has been transfered to the GPU yet.
		*/
		virtual bool loadResourcesToMainMemory() = 0;
		/**
			Second step in the initialization. In this stage:
				- we will create the required data structures in the GPU through OpenGL calls (e.g. attributes, buffers, textures handlers, etc...).
				- we will also copy the data from maiin memory into GPU memory.
				- our shaders will also be compiled and ready to run in the GPU.
				- at the end of the process, our content is ready to be rendered.

		*/
		virtual bool allocateOpenGLResources() = 0;

		/**
			Actual method to render the content. The parameters describe the position of the camera (see View matrix V), and its viewing volume/frustum (using a projection matrix P).
			The terms View Matrix and Projection matrix are extremely important, make sure you understand them. 
		*/
		virtual bool render(glm::mat4 P, glm::mat4 V) {
			if (!isEnabled())return false;
			return true;
		}

		/**
			This method will deallocate the resources used, both from main memory and from the GPU
		*/
		virtual bool unallocateAllResources() = 0;

		/**
			This method sets the OpeGL rendering primitive to use, when interpreting our vertex buffers. This will alow us to render them as triangles, lines, points, etc...
		*/
		inline GLuint setPrimitive(GLuint newPrimitive) {
			GLuint oldPrimitive = renderPrimitive;
			renderPrimitive = newPrimitive;
			return oldPrimitive; //In case the caller wanted to know how we were rendering before (e.g. to restore it later)
		}

		/**
		Return the current rendering primitive used (GL_TRIANGLES, GL_POINTS, etc...)
		*/
		inline GLuint getRenderPrimitive() { return renderPrimitive; }

		/**
			Return a copy of the Bounding box for the renderable. The bounding bos is aligned to the object's local system of reference (not world). 
			This can be combined with rayCastingCollision and isInsideBB methods in 3DUI_Utils, to interact with objects. 
		*/
		inline BoundingBox getLocalBoundingBox() {
			return this->bb;
		}
	};
};
#endif
