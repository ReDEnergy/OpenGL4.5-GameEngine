#include "Transform.h"

#include <include/glm_utils.h>

#include <iostream>

Transform::Transform()
{
	_localPosition = glm::vec3(0);
	_worldPosition = glm::vec3(0);
	_worldRotation = glm::quat();
	_invWorldRotation = glm::quat();

	_localScale = glm::vec3(1.0f);

	_rotateSpeed = 1;
	_moveSpeed = 1;
	_scaleSpeed = 0.02f;

	Init();
}

Transform::Transform(const Transform &transform)
{
	_localPosition = transform._localPosition;
	_worldPosition = transform._worldPosition;
	_worldRotation = transform._worldRotation;
	_invWorldRotation = glm::inverse(_worldRotation);

	_localScale = transform._localScale;

	_moveSpeed = transform._moveSpeed;
	_rotateSpeed = transform._rotateSpeed;
	_scaleSpeed = transform._scaleSpeed;

	Init();
}

Transform::~Transform() {
}

float Transform::DistanceTo(Transform * transform)
{
	return glm::distance(_worldPosition, transform->GetWorldPosition());
}

float Transform::DistanceTo(const glm::vec3 & position)
{
	return glm::distance(_worldPosition, position);
}

float Transform::Distance2To(Transform * transform)
{
	return glm::distance2(_worldPosition, transform->GetWorldPosition());
}

float Transform::Distance2To(const glm::vec3 & position)
{
	return glm::distance2(_worldPosition, position);
}

glm::vec3 Transform::GetRelativePositionOf(const Transform & transform)
{
	return glm::rotate(_invWorldRotation, transform._worldPosition - _worldPosition);
}

void Transform::Init()
{
	_relativeRotation = glm::quat();

	_parentNode = nullptr;
	_motionState = false;
	_updateHierarchy = true;
	_modelIsOutdated = true;

	UpdateWorldModel();
}

void Transform::SetHierarchyUpdate(bool value)
{
	_updateHierarchy = value;
}

void Transform::ClearMotionState()
{
	_motionState = false;
	for (auto C : _childNodes) {
		C->ClearMotionState();
	}
}

bool Transform::GetMotionState() const
{
	return _motionState;
}

void Transform::ComputeWorldModel()
{
	_worldModel = glm::toMat4(_worldRotation);
	_worldModel[0] *= _localScale[0];
	_worldModel[1] *= _localScale[1];
	_worldModel[2] *= _localScale[2];
	UpdateModelPosition();
	_modelIsOutdated = false;
}

void Transform::UpdateWorldModel()
{
	_modelIsOutdated = true;
	_motionState = true;
}

void Transform::UpdateModelPosition()
{
	memcpy(&_worldModel[3], &_worldPosition, 12);
	_motionState = true;
}

void Transform::ForceUpdate()
{
	ComputeWorldModel();
}

void Transform::Translate(const glm::vec3 &offset)
{
	_worldPosition += offset;
	SetWorldPosition(_worldPosition);
	UpdateWorldModel();
}

void Transform::Move(const glm::vec3 &dir, float deltaTime) {
	_worldPosition += glm::normalize(dir) * _moveSpeed * deltaTime;
	SetWorldPosition(_worldPosition);
	UpdateWorldModel();
}

glm::vec3 Transform::GetLocalPosition() const
{
	return _localPosition;
}

glm::vec3 Transform::GetWorldPosition() const
{
	return _worldPosition;
}

glm::quat Transform::GetWorldRotation() const
{
	return _worldRotation;
}

glm::quat Transform::GetRelativeRotation() const
{
	return _relativeRotation;
}

glm::vec3 Transform::GetRotationEulerRad() const
{
	return glm::eulerAngles(_worldRotation);
}

glm::vec3 Transform::GetRotationEuler360() const
{
	return glm::eulerAngles(_worldRotation) * TO_DEGREES;
}

glm::vec3 Transform::GetScale() const
{
	return _localScale;
}

const glm::mat4& Transform::GetModel()
{
	if (_modelIsOutdated)
		ComputeWorldModel();
	return _worldModel;
}

glm::vec3 Transform::GetLocalOXVector() const
{
	return glm::rotate(_worldRotation, glm::vec3(1, 0, 0));
}

glm::vec3 Transform::GetLocalOYVector() const
{
	return glm::rotate(_worldRotation, glm::vec3(0, 1, 0));
}

glm::vec3 Transform::GetLocalOZVector() const
{
	return glm::rotate(_worldRotation, glm::vec3(0, 0, 1));
}

float Transform::GetMoveSpeed() const
{
	return _moveSpeed;
}

float Transform::GetScaleSpeed() const
{
	return _scaleSpeed;
}

float Transform::GetRotationSpeed() const
{
	return _rotateSpeed;
}

void Transform::Rotate(glm::quat offset)
{
	SetWorldRotation(_worldRotation * offset);
}

void Transform::RotateWorldOX(float deltaTime) {
	auto diff = glm::AxisAngle(1, 0, 0, _rotateSpeed * deltaTime);
	SetWorldRotation(diff * _worldRotation);
}

void Transform::RotateWorldOY(float deltaTime) {
	auto diff = glm::AxisAngle(0, 1, 0, _rotateSpeed * deltaTime);
	SetWorldRotation(diff * _worldRotation);
}

void Transform::RotateWorldOZ(float deltaTime) {
	auto diff = glm::AxisAngle(0, 0, 1, _rotateSpeed * deltaTime);
	SetWorldRotation(diff * _worldRotation);
}

void Transform::RotateLocalOX(float deltaTime) {
	auto diff = glm::AxisAngle(1, 0, 0, _rotateSpeed * deltaTime);
	SetWorldRotation(_worldRotation * diff);
}

void Transform::RotateLocalOY(float deltaTime) {
	auto diff = glm::AxisAngle(0, 1, 0, _rotateSpeed * deltaTime);
	SetWorldRotation(_worldRotation * diff);
}

void Transform::RotateLocalOZ(float deltaTime) {
	auto diff = glm::AxisAngle(0, 0, 1, _rotateSpeed * deltaTime);
	SetWorldRotation(_worldRotation * diff);
}

void Transform::Copy(const Transform & source)
{
	_localScale = source.GetScale();
	SetWorldRotation(source.GetWorldRotation());
	SetWorldPosition(source.GetWorldPosition());
}

void Transform::SetMoveSpeed(float speed)
{
	_moveSpeed = speed;
}

void Transform::SetScaleSpeed(float speed)
{
	_scaleSpeed = speed;
}

void Transform::SetRotationSpeed(float speed)
{
	_rotateSpeed = speed;
}

void Transform::SetWorldPosition(glm::vec3 position)
{
	_worldPosition = std::move(position);
	UpdateLocalPosition();
	UpdateChildsPosition();
	UpdateModelPosition();
}

void Transform::SetLocalPosition(glm::vec3 position)
{
	_localPosition = std::move(position);
	UpdateWorldPosition();
	UpdateChildsPosition();
	UpdateModelPosition();
}

void Transform::UpdateWorldPosition()
{
	_worldPosition = _parentNode ? (_parentNode->_worldPosition + glm::rotate(_parentNode->_worldRotation, _localPosition)) : _localPosition;
}

void Transform::UpdateLocalPosition()
{
	_localPosition = _parentNode ? glm::rotate(_parentNode->_invWorldRotation, _worldPosition - _parentNode->_worldPosition) : _worldPosition;
}

void Transform::UpdateChildsPosition()
{
	if (!_updateHierarchy) return;
	for (auto c : _childNodes) {
		c->SetLocalPosition(c->_localPosition);
	}
}

void Transform::SetWorldRotation(const glm::vec3 &eulerAngles360)
{
	SetWorldRotation(glm::quat(eulerAngles360 * TO_RADIANS));
}

void Transform::SetWorldRotation(glm::quat rotationQ)
{
	_worldRotation = std::move(rotationQ);
	_invWorldRotation = glm::inverse(_worldRotation);
	UpdateChildrenRotation();
	UpdateRelativeRotation();
	UpdateWorldModel();
}

void Transform::UpdateRelativeRotation()
{
	glm::quat invParentRot = _parentNode ? _parentNode->_invWorldRotation : glm::quat();
	_relativeRotation = invParentRot * _worldRotation;
}

void Transform::SetRelativeRotation(const glm::vec3 &eulerAngles360)
{
	SetRelativeRotation(glm::quat(eulerAngles360 * TO_RADIANS));
}

void Transform::SetRelativeRotation(const glm::quat &localRotationQ)
{
	auto worldRotation = _parentNode ? _parentNode->_worldRotation * localRotationQ : localRotationQ;
	SetWorldRotation(worldRotation);
}

void Transform::SetWorldRotationAndScale(const glm::quat &rotationQ, glm::vec3 scale)
{
	_localScale = std::move(scale);
	SetWorldRotation(rotationQ);
}

void Transform::UpdateWorldInfo()
{
	_worldRotation = _parentNode->_worldRotation * _relativeRotation;
	_worldPosition = _parentNode->_worldPosition + glm::rotate(_parentNode->_worldRotation, _localPosition);
	_invWorldRotation = glm::inverse(_worldRotation);
	UpdateChildrenRotation();
	UpdateWorldModel();
}

void Transform::UpdateChildrenRotation()
{
	if (!_updateHierarchy) return;
	for (auto c : _childNodes) {
		c->UpdateWorldInfo();
	}
}

void Transform::SetScale(glm::vec3 scale)
{
	_localScale = std::move(scale);
	UpdateWorldModel();
}

void Transform::Scale(float deltaTime) {
	_localScale += _scaleSpeed * glm::vec3(deltaTime);
	UpdateWorldModel();
}

void Transform::AddChild(Transform * transform)
{
	_childNodes.push_back(transform);
	transform->_parentNode = this;
	transform->SetWorldPosition(transform->_worldPosition);
	transform->SetWorldRotation(transform->_worldRotation);
}

void Transform::RemoveChild(Transform * transform)
{
	_childNodes.remove(transform);
	transform->_parentNode = nullptr;
	transform->SetWorldPosition(transform->_worldPosition);
	transform->SetWorldRotation(transform->_worldRotation);
}