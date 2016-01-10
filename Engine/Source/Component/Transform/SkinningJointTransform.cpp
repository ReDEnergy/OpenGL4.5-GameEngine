#include "SkinningJointTransform.h"

#include <include/glm_utils.h>

SkinningJointTransform::SkinningJointTransform()
{
	fakeModel = false;
}

SkinningJointTransform::~SkinningJointTransform()
{
}

glm::vec3 SkinningJointTransform::GetWorldPosition() const
{
	return glm::vec3(_worldModel[3]);
}

void SkinningJointTransform::UpdateChildsPosition()
{
}

void SkinningJointTransform::UpdateChildrenRotation(glm::quat diffQ)
{
}

void SkinningJointTransform::SetFakeModel(const glm::mat4 & model)
{
	fakeModel = true;
	_worldModel = model;
	_motionState = true;
}
