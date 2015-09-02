//#include <pch.h>
#include "LimitedTransform.h"

#include <include/glm_utils.h>

LimitedTransform::LimitedTransform() {
	limitMax = glm::vec3(60);
	limitMin = glm::vec3(-60);
}

LimitedTransform::LimitedTransform(const Transform &transform)
{
}

LimitedTransform::~LimitedTransform() {
}

void LimitedTransform::SetWorldRotation(glm::quat rotationQ)
{
	glm::vec3 euler = glm::eulerAngles(rotationQ);

	for (int i = 0; i < 3; i++) {
		if (euler[i] > limitMax[i]) {
			euler[i] = limitMax[i];
		}
		if (euler[i] < limitMin[i]) {
			euler[i] = limitMin[i];
		}
	}

	glm::quat rotq = glm::quat(euler * TO_RADIANS);

	//cout << "rot: " << euler << endl;
	//cout << "quat: " << rotationQ << endl;
	//cout << "nquat: " << rotq << endl;
	//cout << endl;

	if (_childNodes.size()) {
		glm::quat diffQ = rotq * glm::inverse(_worldRotation);
		for (auto c : _childNodes) {
			c->UpdateLocalRotation(diffQ);
		}
	}
	_worldRotation = rotq;
	UpdateWorldModel();
}