#include <OpenGLFramework\Components\RenderComponent\RenderableVisitor\RenderableVisitor.h>
#include <OpenGLFramework\SceneNodes\IVirtualObject.h>
#include <OpenGLFramework\SceneNodes\IScenenode.h>
#include <OpenGLFramework\Components\RenderComponent\OpenGL_Renderable.h>

using namespace OpenGLFramework;
//It is OK to use namespaces in the context of a .cpp file, but do not do it in a .h
//Otherwise, anyone #include'ing the .h file also inherits the "using namespace something;" directive
// When a piece of software uses many libraries, with many namesapces, this can lead to collisions in class names, methods, etc...
//BONUS: Not using namespaces, you will know which library is giving you the functionality (i.e. I am using glm, stl, CImg, etc...)
//CONS: You will be writing lot's of <namespace>::<method> (e.g. glm::normalise(...), std::vector<int>, etc.)
OpenGLFramework::RenderableVisitor::RenderableVisitor(glm::mat4 P, glm::mat4 V) : P(P), V(V) { ; }

bool OpenGLFramework::RenderableVisitor::visitVirtualObject(OpenGLFramework::IVirtualObject* vo) {
	//1. We get all renderable components
	std::list<IComponent*> l=vo->getAllComponentsOfType("Renderable");
	//2. Traverse them and ask them to render
	std::list<IComponent*>::iterator it = l.begin();
	for (; it != l.end(); it++) {
		OpenGL_Renderable* renderable = dynamic_cast<OpenGL_Renderable*>(*it);
		if (renderable && renderable->isEnabled())//dynamic_cast succeeded --> it is the right type of component
			renderable->render(P, V);
	}
	return true;
}
bool OpenGLFramework::RenderableVisitor::visitSceneNode(OpenGLFramework::ISceneNode* vo) {
	//1. Get all the children of the node
	std::map<unsigned int, IVirtualObject*>& children = vo->getAllChildren();
	//2. ... and visit them
	std::map<unsigned int, IVirtualObject*>::iterator it = children.begin();
	for (; it != children.end(); it++)
		it->second->visit(*((ISceneVisitor*)this));

	return true;
}
