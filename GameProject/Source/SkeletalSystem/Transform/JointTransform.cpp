#include <pch.h>
#include "JointTransform.h"

#include <include/glm_utils.h>
#include <Math/quaterion.h>

JointTransform::JointTransform()
{
	direction = glm::vec3(0, 1, 0);
	coneAngle = 90;
}

JointTransform::~JointTransform()
{
}

glm::vec3 JointTransform::GetConstraintDirection() const
{
	return direction;
}

float JointTransform::GetConstraintAngle() const
{
	return coneAngle;
}

void JointTransform::LimitAxisRotation(glm::vec3 axis)
{
	constraintAxis = axis;
}

void JointTransform::SetConstraint(glm::vec3 direction, float coneAngle)
{
	this->direction = glm::normalize(direction);
	this->coneAngle = coneAngle;
}

void JointTransform::SetChildrenRotation(glm::quat rotationQ)
{
	RotateChildren(rotationQ);
}

//void JointTransform::SetRelativeRotation(glm::quat rotationQ)
//{
//	auto quat = ClosestQuat(rotationQ, constraintAxis);
//	Transform::SetWorldRotation(quat);
//}

void JointTransform::SetWorldRotation(glm::quat rotationQ)
{
	auto parentRot = _parentNode ? _parentNode->_worldRotation : glm::quat();
	auto relativeRot = glm::inverse(parentRot) * rotationQ;
	_relativeRotation = ClosestQuatX(relativeRot);
	_worldRotation = parentRot * _relativeRotation;
	UpdateChildrenRotation();
	UpdateWorldModel();
}

void JointTransform::RotateChildren(glm::quat rotationQ)
{
	auto save = _worldRotation;
	_worldRotation = rotationQ;

	UpdateChildrenRotation();

	// Revert rotation and compute the new local rotation for child-nodes
	_worldRotation = save;

	for (auto child : _childNodes) {
		child->UpdateLocalPosition();
	}

	_motionState = true;
}

//void JointTransform::UpdateChildRotation(const glm::quat &diffQ)
//{
//	_worldRotation = diffQ * _worldRotation;
//	_worldPosition = _parentNode->_worldPosition + glm::rotate(_parentNode->_worldRotation, _localPosition);
//	_relativeRotation = glm::inverse(_parentNode->_worldRotation) * _worldRotation;
//	UpdateChildrenRotation(diffQ);
//	UpdateWorldModel();
//}