#include "LimitedTransform.h"

#include <include/glm_utils.h>
#include <Math/quaterion.h>

LimitedTransform::LimitedTransform()
{
	constraintAxis = glm::vec3();
}

LimitedTransform::LimitedTransform(const Transform &transform)
{
}

LimitedTransform::~LimitedTransform() {
}

void LimitedTransform::LimitAxisRotation(glm::vec3 axis)
{
	constraintAxis = axis;
	applyLimits = (constraintAxis != glm::vec3());
	if (axis.x && axis.y == 0 && axis.z == 0) { RotLimitFunc = ClosestQuatX; return; }
	if (axis.y && axis.x == 0 && axis.z == 0) { RotLimitFunc = ClosestQuatY; return; }
	if (axis.z && axis.x == 0 && axis.y == 0) { RotLimitFunc = ClosestQuatZ; return; }
	if (axis.x == 0 && axis.y && axis.z) { RotLimitFunc = ClosestQuatYZ; return; }
	if (axis.y == 0 && axis.x && axis.z) { RotLimitFunc = ClosestQuatZX; return; }
	if (axis.z == 0 && axis.x && axis.y) { RotLimitFunc = ClosestQuatXY; return; }

	// if no match was found do not restrict rotations
	applyLimits = false;
}

void LimitedTransform::SetWorldRotation(glm::quat rotationQ)
{
	if (applyLimits)
	{
		auto parentRot = _parentNode ? _parentNode->_worldRotation : glm::quat();
		auto relativeRot = glm::inverse(parentRot) * rotationQ;
		_relativeRotation = RotLimitFunc(relativeRot);
		_worldRotation = parentRot * _relativeRotation;
		UpdateChildrenRotation();
		UpdateWorldModel();
	}
	else
	{
		Transform::SetWorldRotation(rotationQ);
	}
}

void LimitedTransform::SetRelativeRotation(const glm::quat & rotationQ)
{
	if (applyLimits)
	{
		auto rotation = RotLimitFunc(rotationQ);
		Transform::SetRelativeRotation(rotation);
	}
	else
	{
		Transform::SetRelativeRotation(rotationQ);
	}
}
