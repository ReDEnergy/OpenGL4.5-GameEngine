#include "Serialization.h"
#include <include/glm_utils.h>

DLLExport void Serialization::ReadTransform(pugi::xml_node node, Transform & T)
{
	if (!node)
		return;

	pugi::xml_node prop;

	prop = node.child("position");
	if (prop)
		T.SetWorldPosition(glm::ExtractVector<glm::vec3>(prop.text().get()));

	prop = node.child("rotation");
	if (prop) {
		glm::vec3 rotation = glm::ExtractVector<glm::vec3>(prop.text().get());
		T.SetWorldRotation(rotation);
	}
	prop = node.child("quaternion");
	if (prop) {
		glm::quat quat = glm::ExtractVector<glm::quat>(prop.text().get());
		T.SetWorldRotation(quat);
	}

	prop = node.child("scale");
	if (prop)
		T.SetScale(glm::ExtractVector<glm::vec3>(prop.text().get()));
}
