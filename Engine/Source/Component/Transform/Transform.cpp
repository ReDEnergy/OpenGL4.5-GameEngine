//#include <pch.h>
#include "Transform.h"

#include <include/glm_utils.h>

Transform::Transform() {
	_localPosition = glm::vec3(0);
	_worldPosition = glm::vec3(0);
	_worldRotation = glm::quat(1.0f, 0, 0, 0);
	_localScale = glm::vec3(1.0f);

	_rotateSpeed = 1;
	_moveSpeed = 1;
	_scaleSpeed = 0.02f;

	Init();
}

Transform::Transform(const Transform &transform) {
	_localPosition = transform._localPosition;
	_worldPosition = transform._worldPosition;
	_worldRotation = transform._worldRotation;
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

void Transform::Init()
{
	_parentNode = nullptr;
	_motionState = false;
	UpdateWorldModel();
}

void Transform::ClearMotionState()
{
	_motionState = false;
}

bool Transform::GetMotionState() const
{
	return _motionState;
}

// World Scaling: First apply rotation then scale the matrix
//_worldModel = glm::scale(glm::mat4(1.0), _worldScale);
//_worldModel *= glm::toMat4(_worldRotation);

void Transform::UpdateWorldModel()
{
	_worldModel = glm::toMat4(_worldRotation);
	_worldModel[0] *= _localScale[0];
	_worldModel[1] *= _localScale[1];
	_worldModel[2] *= _localScale[2];
	UpdatePosition();
}

void Transform::UpdatePosition()
{
	_worldModel[3][0] = _worldPosition[0];
	_worldModel[3][1] = _worldPosition[1];
	_worldModel[3][2] = _worldPosition[2];
	_motionState = true;
}

void Transform::UpdateChildsPosition()
{
	for (auto c : _childNodes) {
		c->SetLocalPosition(c->_localPosition);
	}
}

void Transform::UpdateChildsRotation(glm::quat diffQ)
{
	for (auto c : _childNodes) {
		c->UpdateLocalRotation(diffQ);
	}
}

void Transform::UpdateLocalPosition()
{
	if (!_parentNode)
		return;
	SetLocalPosition(_worldPosition - _parentNode->_worldPosition);
}

void Transform::Move(const glm::vec3 dir, float deltaTime) {
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

glm::vec3 Transform::GetRotationEuler() const
{
	return glm::eulerAngles(_worldRotation) * TO_RADIANS;
}

glm::vec3 Transform::GetScale() const
{
	return _localScale;
}

glm::mat4 Transform::GetModel() const
{
	return _worldModel;
}

glm::vec3 Transform::GetLocalOXVector()
{
	return glm::rotate(_worldRotation, glm::vec3(1, 0, 0));
}

glm::vec3 Transform::GetLocalOYVector()
{
	return glm::rotate(_worldRotation, glm::vec3(0, 1, 0));
}

glm::vec3 Transform::GetLocalOZVector()
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

void Transform::RotateWorldOX(float deltaTime) {
	SetWorldRotation(glm::axis_angle(1, 0, 0, _rotateSpeed * deltaTime) * _worldRotation);
}

void Transform::RotateWorldOY(float deltaTime) {
	SetWorldRotation(glm::axis_angle(0, 1, 0, _rotateSpeed * deltaTime) * _worldRotation);
}

void Transform::RotateWorldOZ(float deltaTime) {
	SetWorldRotation(glm::axis_angle(0, 0, 1, _rotateSpeed * deltaTime) * _worldRotation);
}

void Transform::RotateLocalOX(float deltaTime) {
	SetWorldRotation(_worldRotation * glm::axis_angle(1, 0, 0, _rotateSpeed * deltaTime));
}

void Transform::ForceUpdate()
{
	UpdateWorldModel();
}

void Transform::RotateLocalOY(float deltaTime) {
	SetWorldRotation(_worldRotation * glm::axis_angle(0, 1, 0, _rotateSpeed * deltaTime));
}

void Transform::RotateLocalOZ(float deltaTime) {
	SetWorldRotation(_worldRotation * glm::axis_angle(0, 0, 1, _rotateSpeed * deltaTime));
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

void Transform::SetLocalPosition(glm::vec3 position)
{
	_localPosition = position;
	_worldPosition = _parentNode ? _localPosition + _parentNode->_worldPosition : _localPosition;
	UpdateChildsPosition();
	UpdatePosition();
}

void Transform::SetWorldPosition(glm::vec3 position)
{
	_worldPosition = position;
	_localPosition = _parentNode ? position - _parentNode->_worldPosition : position;
	UpdateChildsPosition();
	UpdatePosition();
}

void Transform::UpdateLocalRotation(const glm::quat &diffQ)
{
	_worldRotation = diffQ * _worldRotation;
	_localPosition = glm::rotate(diffQ, _localPosition);
	_worldPosition = _localPosition + _parentNode->_worldPosition;
	UpdateChildsRotation(diffQ);
	UpdateWorldModel();
}

void Transform::SetWorldRotation(glm::vec3 eulerAngles360) {
	glm::vec3 worldEuler = glm::vec3(eulerAngles360) * TO_RADIANS;
	SetWorldRotation(glm::quat(worldEuler));
}

void Transform::SetWorldRotation(glm::quat rotationQ)
{
	if (_childNodes.size()) {
		glm::quat diffQ = rotationQ * glm::inverse(_worldRotation);
		UpdateChildsRotation(diffQ);
	}
	_worldRotation = rotationQ;
	UpdateWorldModel();
}

void Transform::SetWorldRotationAndScale(glm::quat rotationQ, glm::vec3 scale)
{
	_localScale = scale;
	SetWorldRotation(rotationQ);
}

void Transform::SetScale(glm::vec3 scale)
{
	_localScale = scale;
	UpdateWorldModel();
}

void Transform::Scale(float deltaTime) {
	_localScale += _scaleSpeed * glm::vec3(deltaTime);
	UpdateWorldModel();
}

void Transform::AddChild(Transform * transform)
{
	_childNodes.push_back(transform);
	transform->UpdateLocalPosition();
}

void Transform::RemoveChild(Transform * transform)
{
	_childNodes.remove(transform);
}

void Transform::SetParent(Transform * transform)
{
	_parentNode = transform;
}
