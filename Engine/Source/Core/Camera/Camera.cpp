#include "Camera.h"

#include <iostream>

#include <include/gl.h>
#include <include/glm_utils.h>
#include <include/math.h>

#include <InputComponent/CameraInput.h>

#include <Component/AABB.h>
#include <Component/Mesh.h>
#include <Component/Renderer.h>
#include <Component/ObjectInput.h>
#include <Component/Transform/Transform.h>

#include <GPU/Shader.h>

#include <Lighting/FrustumSplit.h>

#include <Manager/Manager.h>
#include <Manager/ResourceManager.h>
#include <Manager/RenderingSystem.h>

#include <Utils/OpenGL.h>
#include <Utils/3D.h>

using namespace std;

Camera::Camera()
	: GameObject("camera")
{
	Init();
}

Camera::~Camera() {}

void Camera::Init()
{
	frustum = nullptr;
	type = CameraType::FirstPerson;

	// Default perspective
	zNear = 0.01f;
	zFar = 50;
	FoVy = 40;
	aspectRatio = 1.6f;

	// not used for now
	limitUp = float(RADIANS(-95));
	limitDown = float(RADIANS(-95));

	minSpeed = 0.1f;
	maxSpeed = 100.0f;

	sensitivityOX = 0.002f;
	sensitivityOY = 0.002f;

	transform->SetMoveSpeed(20.0f);
	transform->SetRotationSpeed(50);
	renderer->SetCastShadow(false);

	SetMesh(Manager::GetResource()->GetMesh("box"));

	ComputeFrustum();
	Update();
}

void Camera::Update()
{
	GameObject::Update();

	if (transform->GetMotionState())
	{
		glm::vec3 target = transform->GetWorldPosition() - transform->GetLocalOZVector();
		auto up = transform->GetLocalOYVector();
		View = glm::lookAt(transform->GetWorldPosition(), target, up);
	}
}

void Camera::UpdateSpeed(float offset)
{
	float speed = transform->GetMoveSpeed() + offset;
	if (speed <= minSpeed || speed >= maxSpeed)
		return;
	transform->SetMoveSpeed(speed);
}

/*
 * First Person Camera View
 */
void Camera::SetYaw(float deltaAngle)
{
	transform->RotateWorldOY(deltaAngle);
}

// TODO - limit PITCH
void Camera::UpdatePitch(float deltaAngle)
{
	transform->RotateLocalOX(deltaAngle);
}

void Camera::UpdateRoll(float deltaAngle)
{
	transform->RotateLocalOZ(deltaAngle);
}

glm::mat4 Camera::GetViewMatrix() const
{
	return View;
}

void Camera::RotateOX(float deltaTime)
{
	if (deltaTime == 0)
		return;
	UpdatePitch(deltaTime * sensitivityOX);
}

void Camera::RotateOY(float deltaTime)
{
	if (deltaTime == 0)
		return;
	 SetYaw(deltaTime * sensitivityOY);
}

void Camera::RotateOZ(float deltaTime) {

}

/*
 * Move Camera
 */

void Camera::MoveForward(float deltaTime)
{
	glm::vec3 forward = -transform->GetLocalOZVector();
	glm::vec3 front = glm::normalize(glm::vec3(forward.x, 0, forward.z));
	transform->Move(front, deltaTime);
}

void Camera::MoveBackward(float deltaTime)
{
	MoveForward(-deltaTime);
}

void Camera::MoveLeft(float deltaTime)
{
	transform->Move(-transform->GetLocalOXVector(), deltaTime);
}

void Camera::MoveRight(float deltaTime)
{
	transform->Move(transform->GetLocalOXVector(), deltaTime);
}

void Camera::MoveUp(float deltaTime)
{
	transform->Move(glm::vec3_up, deltaTime);
}

void Camera::MoveDown(float deltaTime)
{
	MoveUp(-deltaTime);
}

void Camera::MoveInDirection(glm::vec3 dir, float deltaTime)
{
	transform->Move(dir, deltaTime);
}

// Move camera to a certain position
void Camera::SetPosition(glm::vec3 pos) {
	transform->SetWorldPosition(pos);
	Camera::Update();
}

// Print information about camera
void Camera::Log() const
{
	cout.precision(2);
	cout << "Camera =>" << endl;
	cout << "Rotation: " << transform->GetRotationEulerRad() << endl;
	cout << "Position: " << transform->GetWorldPosition() << endl;
	cout << "Forward : " << -transform->GetLocalOZVector() << endl;
	cout << "Up      : " << transform->GetLocalOYVector() << endl;
	cout << "--------------------------------------------------" << endl;
}

void Camera::BindMVP(const Shader * shader) const
{
	BindPosition(shader->loc_eye_pos);
	BindViewMatrix(shader->loc_view_matrix);
	BindProjectionMatrix(shader->loc_projection_matrix);
}

void Camera::BindPosition(int location) const
{
	glm::vec3 pos = transform->GetWorldPosition();
	glUniform3f(location, pos.x, pos.y, pos.z);
};

void Camera::BindViewMatrix(int location) const
{
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(View));
};

void Camera::BindProjectionMatrix(int location) const
{
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(Projection));
};

///////////////////////////////////////////////////////////////////////////////
// Frustum methods

void Camera::SetPerspective(float FoVy, float aspectRatio, float zNear, float zFar)
{
	isPerspective = true;
	this->zFar = zFar;
	this->zNear = zNear;
	this->aspectRatio = aspectRatio;
	this->FoVy = FoVy;
	Projection = glm::perspective(RADIANS(FoVy), aspectRatio, zNear, zFar);
	ComputeFrustum();
}

void Camera::SetOrthgraphic(float width, float height, float zNear, float zFar)
{
	isPerspective = false;
	this->zFar = zFar;
	this->zNear = zNear;
	this->aspectRatio = width / height;
	this->ortographicWidth = width;
	Projection = glm::ortho(-width/2, width/2, -height/2, height/2, zNear, zFar);
}

void Camera::SetProjection(const ProjectionInfo & PI)
{
	if (PI.isPerspective) {
		SetPerspective(PI.FoVy, PI.aspectRatio, PI.zNear, PI.zFar);
	}
	else {
		SetOrthgraphic(PI.width, PI.height, PI.zNear, PI.zFar);
	}
}

ProjectionInfo Camera::GetProjectionInfo() const
{
	ProjectionInfo P;
	P.FoVy = FoVy;
	P.aspectRatio = aspectRatio;
	P.zFar = zFar;
	P.zNear = zNear;
	P.isPerspective = isPerspective;
	P.width = ortographicWidth;
	P.height = ortographicWidth / aspectRatio;
	return P;
}

float Camera::GetFieldOfViewY() const
{
	return FoVy;
}

float Camera::GetFieldOfViewX() const
{
	return FoVy * aspectRatio;
}

bool Camera::ColidesWith(GameObject * object)
{
	return true;
}

void Camera::ComputeFrustum()
{
	if (frustum == nullptr) {
		frustum = new GameObject();
		frustum->transform = transform;
		auto fmesh = new Mesh();

		for (int i = 0; i<8; i++) {
			fmesh->normals.push_back(glm::vec3(0, 1, 0));
		}

		// Not needed since creates triangles on the zFar and zNear planes
		//Utils3D::PushQuadTriangle(indices, 0, 1, 2, 3);
		//Utils3D::PushQuadTriangle(indices, 7, 6, 5, 4);

		Utils3D::PushQuadTriangle(fmesh->indices, 4, 5, 1, 0);
		Utils3D::PushQuadTriangle(fmesh->indices, 6, 7, 3, 2);
		Utils3D::PushQuadTriangle(fmesh->indices, 4, 0, 3, 7);
		Utils3D::PushQuadTriangle(fmesh->indices, 5, 6, 2, 1);

		fmesh->positions = Utils3D::GetPerspectiveSection(zNear, FoVy, aspectRatio);
		auto points = Utils3D::GetPerspectiveSection(zFar, FoVy, aspectRatio);
		for (auto point : points) {
			fmesh->positions.push_back(point);
		}
		fmesh->InitFromData();
		frustum->SetMesh(fmesh);
	}
	else {
		auto fmesh = frustum->GetMesh();
		fmesh->positions = Utils3D::GetPerspectiveSection(zNear, FoVy, aspectRatio);
		auto points = Utils3D::GetPerspectiveSection(zFar, FoVy, aspectRatio);
		for (auto point : points) {
			fmesh->positions.push_back(point);
		}
		fmesh->InitFromData();
		frustum->SetMesh(fmesh);
	}
}

void Camera::RenderDebug(const Shader *shader) const
{
	Manager::RenderSys->Set(RenderState::WIREFRAME, true);
	Manager::RenderSys->SetGlobalCulling(OpenGL::CULL::NONE);

	frustum->Render(shader);

	Manager::RenderSys->Revert(RenderState::WIREFRAME);
	Manager::RenderSys->DisableGlobalCulling();
}

void Camera::BindProjectionDistances(const Shader *shader) const
{
	glUniform1f(shader->loc_z_far, zFar);
	glUniform1f(shader->loc_z_near, zNear);
}