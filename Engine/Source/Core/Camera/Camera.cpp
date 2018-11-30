#include "Camera.h"

#include <iostream>

#include <include/gl.h>
#include <include/glm_utils.h>
#include <include/math.h>

#include <InputComponent/CameraInput.h>

#include <Component/AABB.h>
#include <Component/Mesh.h>
#include <Component/MeshRenderer.h>
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
	projection.zNear = 0.01f;
	projection.zFar = 50;
	projection.FoVy = 40;
	projection.aspectRatio = 1.6f;

	// not used for now
	limitUp = float(RADIANS(-95));
	limitDown = float(RADIANS(-95));

	minSpeed = 0.1f;
	maxSpeed = 100.0f;

	sensitivityOX = 0.002f;
	sensitivityOY = 0.002f;

	transform->SetMoveSpeed(20.0f);
	transform->SetRotationSpeed(50);
	transform->SetScale(glm::vec3(0.1f));
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

void Camera::SetPosition(const glm::vec3 & position)
{
	transform->SetWorldPosition(position);
	Camera::Update();
}

void Camera::SetRotation(const glm::quat & worldRotation)
{
	transform->SetWorldRotation(worldRotation);
	Camera::Update();
}

void Camera::SetPositionAndRotation(const glm::vec3 & position, const glm::quat & worldRotation)
{
	transform->SetWorldPosition(position);
	transform->SetWorldRotation(worldRotation);
	Camera::Update();
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
	// TODO - add rotation limits
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

// Print information about camera
void Camera::Log() const
{
	cout.precision(2);
	cout << "[Camera]" << endl;
	cout << "Rotation: " << transform->GetRotationEulerRad() << endl;
	cout << "Position: " << transform->GetWorldPosition() << endl;
	cout << "Forward : " << -transform->GetLocalOZVector() << endl;
	cout << "Up      : " << transform->GetLocalOYVector() << endl;
	cout << "--------------------------------------------------" << endl;
}

void Camera::BindViewProj(const Shader * shader) const
{
	if (shader->loc_eye_pos)
	{
		BindPosition(shader->loc_eye_pos);
	}
		
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
	projection.isPerspective = true;
	projection.zFar = zFar;
	projection.zNear = zNear;
	projection.aspectRatio = aspectRatio;
	projection.FoVy = FoVy;
	Projection = glm::perspective(RADIANS(FoVy), aspectRatio, zNear, zFar);
}

void Camera::SetOrthgraphic(float width, float height, float zNear, float zFar)
{
	projection.isPerspective = false;
	projection.zFar = zFar;
	projection.zNear = zNear;
	projection.aspectRatio = width / height;
	projection.width = width;
	projection.height = height;
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
	return projection;
}

float Camera::GetFieldOfViewY() const
{
	return projection.FoVy;
}

float Camera::GetFieldOfViewX() const
{
	return projection.FoVy * projection.aspectRatio;
}

glm::vec3 Camera::ScreenToWorldRay(int mouseX, int mouseY, glm::ivec2 screenResolution, glm::vec3 &rayOrigin) const
{
	float px = (2.0f * mouseX / screenResolution.x - 1);
	float py = (1 - 2.0f * mouseY / screenResolution.y);

	if (projection.isPerspective)
	{
		rayOrigin = transform->GetWorldPosition();

		float tanY = tan(RADIANS(projection.FoVy / 2));
		float tanX = tanY * projection.aspectRatio;

		float posX = tanX * px;
		float posY = tanY * py;

		// Ray in View Space
		glm::vec3 ray(posX, posY, -1);

		// Ray in View Space
		glm::mat3 invView = glm::inverse(View);
		return invView * ray;
	}
	else
	{
		rayOrigin = transform->GetWorldPosition();

		float offsetX = projection.width / 2 * px;
		float offsetY = projection.height / 2 * py;
		rayOrigin += offsetX * transform->GetLocalOXVector() + offsetY * transform->GetLocalOYVector();

		return -transform->GetLocalOZVector();
	}
}

bool Camera::ColidesWith(GameObject * object)
{
	return true;
}

void Camera::ComputeFrustum()
{
	Mesh *fmesh = nullptr;

	if (frustum == nullptr)
	{
		frustum = new GameObject();
		frustum->transform = transform;
		fmesh = new Mesh();

		for (int i = 0; i < 8; i++) {
			fmesh->normals.push_back(glm::vec3(0, 1, 0));
		}

		// Not needed since creates triangles on the zFar and zNear planes
		//Utils3D::PushQuadTriangle(indices, 0, 1, 2, 3);
		//Utils3D::PushQuadTriangle(indices, 7, 6, 5, 4);

		Utils3D::PushQuadTriangle(fmesh->indices, 4, 5, 1, 0);
		Utils3D::PushQuadTriangle(fmesh->indices, 6, 7, 3, 2);
		Utils3D::PushQuadTriangle(fmesh->indices, 4, 0, 3, 7);
		Utils3D::PushQuadTriangle(fmesh->indices, 5, 6, 2, 1);
	}
	else
	{
		fmesh = const_cast<Mesh*>(frustum->GetMesh());
	}

	fmesh->positions = Utils3D::GetPerspectiveSection(projection.zNear, projection.FoVy, projection.aspectRatio);
	auto points = Utils3D::GetPerspectiveSection(projection.zFar, projection.FoVy, projection.aspectRatio);
	for (auto point : points) {
		fmesh->positions.push_back(point);
	}
	fmesh->InitFromData();
	frustum->SetMesh(fmesh);
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
	glUniform1f(shader->loc_z_far, projection.zFar);
	glUniform1f(shader->loc_z_near, projection.zNear);
}