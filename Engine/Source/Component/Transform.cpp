//#include <pch.h>
#include "Transform.h"
#include <include/math.h>

Transform::Transform() {
	position = glm::vec3(0);
	eulerAngles = glm::vec3(0.0f, 0.0f, 0.0f);
	rotationQ = glm::quat(1.0f, 0, 0, 0);
	scale = glm::vec3(1.0f);

	rotateSpeed = 2.5f;
	moveSpeed = 2.5f;
	scaleSpeed = 0.02f;

	Update();
}

Transform::Transform(const Transform &transform) {
	position = transform.position;
	rotationQ = transform.rotationQ;
	eulerAngles = transform.eulerAngles;
	scale = transform.scale;

	moveSpeed = transform.moveSpeed;
	rotateSpeed = transform.rotateSpeed;
	scaleSpeed = transform.scaleSpeed;

	Update();
}

Transform::~Transform() {
}

void Transform::Update() {
	model = glm::toMat4(rotationQ);
	model[0] *= scale[0];
	model[1] *= scale[1];
	model[2] *= scale[2];
	UpdatePosition();
}

void Transform::UpdatePosition()
{
	model[3][0] = position[0];
	model[3][1] = position[1];
	model[3][2] = position[2];
}

void Transform::Move(const glm::vec3 dir, float deltaTime) {
	position += glm::normalize(dir) * moveSpeed * deltaTime;
	Update();
}

void Transform::RotateRoll(float deltaTime) {
	eulerAngles.x += rotateSpeed * deltaTime;
	rotationQ = glm::quat(eulerAngles);
	Update();
}

void Transform::RotateYaw(float deltaTime) {
	eulerAngles.y += rotateSpeed * deltaTime;
	rotationQ = glm::quat(eulerAngles);
	Update();
}

void Transform::RotatePitch(float deltaTime) {
	eulerAngles.z += rotateSpeed * deltaTime;
	rotationQ = glm::quat(eulerAngles);
	Update();
}

void Transform::SetPosition(glm::vec3 position)
{
	this->position = position;
	UpdatePosition();
}

void Transform::SetRotation(glm::vec3 eulerAngles) {
	this->eulerAngles = glm::vec3(eulerAngles) * (glm::pi<float>() / 180);
	rotationQ = glm::quat(this->eulerAngles);
	Update();
}

void Transform::SetRotation(glm::quat roationQ)
{
	this->rotationQ = roationQ;
	Update();
}

void Transform::SetRotationAndScale(glm::quat roationQ, glm::vec3 scale)
{
	this->rotationQ = roationQ;
	this->scale = scale;
	Update();
}

void Transform::SetScale(glm::vec3 scale)
{
	this->scale = scale;
	Update();
}

void Transform::SetRotationRadians(glm::vec3 eulerAngles) {
	this->eulerAngles = glm::vec3(eulerAngles);
	rotationQ = glm::quat(this->eulerAngles);
	Update();
}

void Transform::Scale(float deltaTime) {
	scale += scaleSpeed * glm::vec3(deltaTime);
	Update();
}

glm::vec3 Transform::GetRotationVector()
{
	glm::vec3 forward;
	forward.x = cos(eulerAngles.y) * cos(eulerAngles.z);
	forward.z = -sin(eulerAngles.y) * cos(eulerAngles.z);
	forward.y = sin(eulerAngles.z);
	return forward;
}
