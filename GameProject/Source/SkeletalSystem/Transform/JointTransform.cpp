#include <pch.h>
#include "JointTransform.h"

#include <include/glm_utils.h>

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

void JointTransform::SetConstraint(glm::vec3 direction, float coneAngle)
{
	this->direction = glm::normalize(direction);
	this->coneAngle = coneAngle;
}

void JointTransform::RotateWorldOX(float deltaTime) {
	RotateChildren(glm::AxisAngle(1, 0, 0, _rotateSpeed * deltaTime) * _worldRotation);
}

void JointTransform::RotateWorldOY(float deltaTime) {
	RotateChildren(glm::AxisAngle(0, 1, 0, _rotateSpeed * deltaTime) * _worldRotation);
}

void JointTransform::RotateWorldOZ(float deltaTime) {
	RotateChildren(glm::AxisAngle(0, 0, 1, _rotateSpeed * deltaTime) * _worldRotation);
}

void JointTransform::RotateLocalOX(float deltaTime) {
	RotateChildren(_worldRotation * glm::AxisAngle(1, 0, 0, _rotateSpeed * deltaTime));
}

void JointTransform::RotateLocalOY(float deltaTime) {
	RotateChildren(_worldRotation * glm::AxisAngle(0, 1, 0, _rotateSpeed * deltaTime));
}

void JointTransform::RotateLocalOZ(float deltaTime) {
	RotateChildren(_worldRotation * glm::AxisAngle(0, 0, 1, _rotateSpeed * deltaTime));
}

void JointTransform::SetChildrenRotation(glm::quat rotationQ)
{
	RotateChildren(rotationQ);
}

void JointTransform::RotateChildren(glm::quat rotationQ)
{
	auto save = _worldRotation;
	_worldRotation = rotationQ;

	glm::quat diffQ = rotationQ * glm::inverse(save);
	UpdateChildrenRotation(diffQ);

	// Revert rotation and compute the new local rotation for childnodes
	_worldRotation = save;

	for (auto child : _childNodes) {
		child->UpdateLocalPosition();
	}

	_motionState = true;
}

void JointTransform::UpdateChildRotation(const glm::quat &diffQ)
{
	_worldRotation = diffQ * _worldRotation;
	_worldPosition = _parentNode->_worldPosition + glm::rotate(_parentNode->_worldRotation, _localPosition);
	_relativeRotation = glm::inverse(_parentNode->_worldRotation) * _worldRotation;
	UpdateChildrenRotation(diffQ);
	UpdateWorldModel();
}