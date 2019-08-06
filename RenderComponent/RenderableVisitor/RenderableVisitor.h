/**
NAME: RenderableVisitor
DESCRIPTION: This is a Visitor subclass used to render the content of our scene.
This kind of Visitors are connected to the components of type OpenGL_Renderable
This vistor implements two different behaviours:
	- For simple objects: simiply calls render, passing them with the camera details
	- For SceneNodes, it will not render anything. However, it will visit all its
	children nodes, causing them to render.
This behaviour ensures all the objects in the tree are visited and all single VirtualObjects rendered.

NEXT OBJECT TO CHECK: Another part of the framework down... I am running out of ideas. 
	Well... the real meat (related to 3D rendering) is mostly in the RenderComponents
	Did you check them already?
*/

#ifndef _OPENGLFRAMEWORK_RENDERABLEVISITOR
#define _OPENGLFRAMEWORK_RENDERABLEVISITOR
#include <OpenGLFramework\OpenGLFRameworkPrerequisites.h>
#include <OpenGLFramework\Components\ISceneVisitor.h>

namespace OpenGLFramework {
	class IVirtualObject;			//Forward declaration
	class ISceneNode;				//Forward declaration

	class RenderableVisitor : public ISceneVisitor{
		glm::mat4 P,  V;			//camera parameters used to render objects
	public:
		RenderableVisitor(glm::mat4 P, glm::mat4 V);
	protected://We extend here the behaviour of the base class
		virtual bool visitVirtualObject(IVirtualObject* vo);
		virtual bool visitSceneNode(ISceneNode* vo);
	};
};
#endif
