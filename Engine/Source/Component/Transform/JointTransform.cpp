//#include <pch.h>
#include "JointTransform.h"

#include <include/glm_utils.h>

JointTransform::JointTransform()
{
}

JointTransform::~JointTransform()
{
}

void JointTransform::UpdateWorldModel()
{
	Transform::UpdateWorldModel();
	//fakeModel = false;
}

void JointTransform::UpdateChildsPosition()
{
}

void JointTransform::UpdateChildsRotation(glm::quat diffQ)
{
}

glm::vec3 JointTransform::GetWorldPosition() const
{
	if (fakeModel)
		return glm::vec3(_worldModel[3]);
	return _worldPosition;
}

void JointTransform::SetFakeModel(const glm::mat4 & model)
{
	fakeModel = true;
	_worldModel = model;
	_motionState = true;
}
