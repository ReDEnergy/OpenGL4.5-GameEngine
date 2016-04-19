#include "Transform.h"

#include <include/glm_utils.h>

Transform::Transform()
{
	_localPosition = glm::vec3(0);
	_worldPosition = glm::vec3(0);
	_worldRotation = glm::quat(1.0f, 0, 0, 0);
	_localScale = glm::vec3(1.0f);

	_rotateSpeed = 1;
	_moveSpeed = 1;
	_scaleSpeed = 0.02f;

	_motionState = false;

	Init();
}

Transform::Transform(const Transform &transform)
{
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
	_relativeRotation = glm::quat();
	_motionState = false;
	UpdateWorldModel();
}

void Transform::ClearMotionState()
{
	_motionState = false;
	for (auto c : _childNodes) {
		c->ClearMotionState();
	}
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
	UpdateModelPosition();
}

void Transform::UpdateModelPosition()
{
	_worldModel[3][0] = _worldPosition[0];
	_worldModel[3][1] = _worldPosition[1];
	_worldModel[3][2] = _worldPosition[2];
	_motionState = true;
}

void Transform::ForceUpdate()
{
	UpdateWorldModel();
}

void Transform::Move(const glm::vec3 offset)
{
	_worldPosition += offset;
	SetWorldPosition(_worldPosition);
	UpdateWorldModel();
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

glm::quat Transform::GetRelativeRotation() const
{
	return _relativeRotation;
}

glm::vec3 Transform::GetRotationEuler() const
{
	return glm::eulerAngles(_worldRotation) * TO_RADIANS;
}

glm::vec3 Transform::GetRotationEuler360() const
{
	return glm::eulerAngles(_worldRotation);
}

glm::vec3 Transform::GetScale() const
{
	return _localScale;
}

const glm::mat4& Transform::GetModel() const
{
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

void Transform::RotateWorldOX(float deltaTime) {
	SetWorldRotation(glm::AxisAngle(1, 0, 0, _rotateSpeed * deltaTime) * _worldRotation);
}

void Transform::RotateWorldOY(float deltaTime) {
	SetWorldRotation(glm::AxisAngle(0, 1, 0, _rotateSpeed * deltaTime) * _worldRotation);
}

void Transform::RotateWorldOZ(float deltaTime) {
	SetWorldRotation(glm::AxisAngle(0, 0, 1, _rotateSpeed * deltaTime) * _worldRotation);
}

void Transform::RotateLocalOX(float deltaTime) {
	SetWorldRotation(_worldRotation * glm::AxisAngle(1, 0, 0, _rotateSpeed * deltaTime));
}

void Transform::RotateLocalOY(float deltaTime) {
	SetWorldRotation(_worldRotation * glm::AxisAngle(0, 1, 0, _rotateSpeed * deltaTime));
}

void Transform::RotateLocalOZ(float deltaTime) {
	SetWorldRotation(_worldRotation * glm::AxisAngle(0, 0, 1, _rotateSpeed * deltaTime));
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
	_worldPosition = position;
	UpdateLocalPosition();
	UpdateChildsPosition();
	UpdateModelPosition();
}

void Transform::SetLocalPosition(glm::vec3 position)
{
	_localPosition = position;
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
	_localPosition = _parentNode ? glm::rotate(glm::inverse(_parentNode->_worldRotation), _worldPosition - _parentNode->_worldPosition) : _worldPosition;
}

void Transform::UpdateChildsPosition()
{
	for (auto c : _childNodes) {
		c->SetLocalPosition(c->_localPosition);
	}
}

void Transform::SetWorldRotation(glm::vec3 eulerAngles360)
{
	glm::vec3 worldEuler = glm::vec3(eulerAngles360) * TO_RADIANS;
	SetWorldRotation(glm::quat(worldEuler));
}

void Transform::SetWorldRotation(glm::quat rotationQ)
{
	auto inv = glm::inverse(_worldRotation);
	_worldRotation = rotationQ;
	glm::quat diffQ = rotationQ * inv;
	UpdateChildrenRotation(diffQ);
	UpdateRelativeRotation();
	UpdateWorldModel();
}

void Transform::UpdateRelativeRotation()
{
	glm::quat parentRot = _parentNode ? _parentNode->_worldRotation : glm::quat();
	_relativeRotation = glm::inverse(parentRot) * _worldRotation;
}

void Transform::SetLocalRotation(glm::quat localRotationQ)
{
	glm::quat localRotation = _parentNode->_worldRotation * localRotationQ;
	SetWorldRotation(localRotation);
}

void Transform::SetWorldRotationAndScale(glm::quat rotationQ, glm::vec3 scale)
{
	_localScale = scale;
	SetWorldRotation(rotationQ);
}

void Transform::UpdateChildRotation(const glm::quat &diffQ)
{
	_worldRotation = diffQ * _worldRotation;
	_worldPosition = _parentNode->_worldPosition + glm::rotate(_parentNode->_worldRotation, _localPosition);
	_relativeRotation = glm::inverse(_parentNode->_worldRotation) * _worldRotation;
	UpdateChildrenRotation(diffQ);
	UpdateRelativeRotation();
	UpdateWorldModel();
}

void Transform::UpdateChildrenRotation(glm::quat diffQ)
{
	for (auto c : _childNodes) {
		c->UpdateChildRotation(diffQ);
	}
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
	transform->SetWorldPosition(transform->_worldPosition);
}

void Transform::RemoveChild(Transform * transform)
{
	_childNodes.remove(transform);
}

void Transform::SetParent(Transform * transform)
{
	_parentNode = transform;
	SetWorldPosition(_worldPosition);
}
