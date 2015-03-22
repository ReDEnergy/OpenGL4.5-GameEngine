//#include <pch.h>
#include "Camera.h"
#include <iostream>

#include <include/glm_utils.h>
#include <include/math.h>

#include <InputComponent/CameraInput.h>

#include <Component/AABB.h>
#include <Component/Mesh.h>
#include <Component/ObjectInput.h>
#include <Component/Transform.h>

#include <GPU/Shader.h>

#include <Lighting/DirectionalLight.h>

#include <Manager/Manager.h>
#include <Manager/DebugInfo.h>
#include <Manager/ResourceManager.h>
#include <Manager/RenderingSystem.h>

#include <Utils/GPU.h>
#include <Utils/3D.h>

Camera::Camera()
{
	Init();
}

Camera::~Camera() {}

void Camera::Init()
{
	forward	= glm::vec3(1.0f, 0.0f, 0.0f);
	right	= glm::vec3(0.0f, 0.0f, 1.0f);
	up		= glm::cross(right, forward);

	// Default perspective
	zNear = 0.1f;
	zFar = 50;
	FOV = 40;
	aspectRatio = 1.6f;
	splits = 1;

	// not used for now
	limitUp = -0.95f * (float) M_PI / 2;
	limitDown = -0.95f * (float) M_PI / 2;

	type = CameraType::FirstPerson;

	minSpeed = 0.1f;
	maxSpeed = 100.0f;

	sensivityOX = 0.002f;
	sensivityOY = 0.002f;

	transform->moveSpeed = 20.0f;

	frustum = nullptr;
	physicalDevice = Manager::Resource->GetGameObject("light-debug");
	physicalDevice->transform = transform;
}

void Camera::Update()
{
	glm::vec3 target = transform->position + forward;
	View = glm::lookAt(transform->position, target, up);
}

/*
 *	Glut Camera Render
 */

void Camera::IncreaseSpeed()
{
	if (transform->moveSpeed > maxSpeed)
		return;
	transform->moveSpeed += 0.2f;
}

void Camera::DecreaseSpeed()
{
	if (transform->moveSpeed <= minSpeed)
		return;
	transform->moveSpeed -= 0.2f;
}

/*
 * First Person Camera View
 */
// Must switch to eulerAngles and quaternion based rotation
void Camera::SetYaw(float deltaAngle)
{
	forward = glm::normalize(glm::RotateOY(forward, deltaAngle));
	right = glm::normalize(glm::RotateOY(right, deltaAngle));
	up = glm::normalize(glm::RotateOY(up, deltaAngle));

	transform->eulerAngles.y = -atan2(forward.z, forward.x) - float(M_PI_2);
	transform->SetRotationRadians(transform->eulerAngles);
}

void Camera::UpdatePitch(float deltaAngle)
{
	transform->eulerAngles.x -= deltaAngle;
	if (transform->eulerAngles.x > M_PI_2 ||
		transform->eulerAngles.x < -M_PI_2) {
		transform->eulerAngles.x += deltaAngle;
		return;
	}

	up = up * cos(deltaAngle) + forward * sin(deltaAngle);
	forward = glm::cross(up, right);

	transform->SetRotationRadians(transform->eulerAngles);
}

void Camera::UpdateRoll(float deltaAngle)
{
	right = right * cos(deltaAngle) + up * sin(deltaAngle);
	up = glm::cross(right, forward);
}

void Camera::RotateOX(float deltaTime)
{
	if (deltaTime == 0)
		return;
	 UpdatePitch(deltaTime * sensivityOX);
}

void Camera::RotateOY(float deltaTime)
{
	if (deltaTime == 0)
		return;
	 SetYaw(deltaTime * sensivityOY);
}

void Camera::RotateOZ(float deltaTime) {

}

/*
 * Move Camera
 */

void Camera::MoveForward(float deltaTime)
{
	glm::vec3 front = glm::normalize(glm::vec3(forward.x, 0, forward.z));
	transform->Move(front, deltaTime);
}

void Camera::MoveBackward(float deltaTime)
{
	MoveForward(-deltaTime);
}

void Camera::MoveRight(float deltaTime)
{
	glm::vec3 side = glm::normalize(glm::vec3(right.x, 0, right.z));
	transform->Move(side, deltaTime);
}

void Camera::MoveLeft(float deltaTime)
{
	MoveRight(-deltaTime);
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

void Camera::Set(glm::vec3 position, glm::vec3 target, glm::vec3 up)
{
	transform->position = position;
	this->forward = target;
	this->up = up;
	Update();
}

// Move camera to a certain position
void Camera::SetPosition(glm::vec3 pos) {
	transform->position = pos;
	transform->Update();
	Update();
}

void Camera::SetDirection(glm::vec3 direction)
{
	direction = glm::normalize(direction);
	float rotateOY = atan2(direction.x, direction.z);
	transform->eulerAngles.y = -rotateOY;
	transform->SetRotationRadians(transform->eulerAngles);
	forward = glm::vec3(direction.x, 0, direction.z);
	up = glm::vec3(0, 1, 0);
	right = glm::cross(forward, up);
	Update();
}


// Print information about camera
void Camera::Log() const
{
	cout.precision(2);
	cout << "Camera =>" << endl;
	cout << "Rotation: " << transform->eulerAngles << endl;
	cout << "Position: " << transform->position << endl;
	cout << "Forward: " << forward << endl;
	cout << "tForward: " << transform->GetRotationVector() << endl;
	cout << "Up: " << up << endl;

	float rotateOY = atan2(forward.z, forward.x);
	cout << "rotateOY: " << rotateOY << endl;
	cout << "--------------------------------------------------" << endl;
}

void Camera::BindPosition(GLint location) const
{
	glUniform3f(location, transform->position.x, transform->position.y, transform->position.z);
};

void Camera::BindViewMatrix(GLint location) const
{
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(View));
};

void Camera::BindProjectionMatrix(GLint location) const
{
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(Projection));
};

///////////////////////////////////////////////////////////////////////////////
// Frustum methods

void Camera::SetPerspective(float FOV, float aspectRatio, float zNear, float zFar)
{
	this->zFar = zFar;
	this->zNear = zNear;
	this->aspectRatio = aspectRatio;
	this->FOV = FOV;
	this->Projection = glm::perspective(FOV, aspectRatio, zNear, zFar);
}

void Camera::SetOrthgraphic(float width, float height, float zNear, float zFar)
{
	this->zFar = zFar;
	this->zNear = zNear;
	this->aspectRatio = width / height;
	this->Projection = glm::ortho(-width/2, width/2, -height/2, height/2, zNear, zFar);
}

// Plane points in world space
void Camera::ComputePerspectiveSection(float distance, vector<glm::vec3>::iterator it) const
{
	glm::vec3 f = glm::vec3(0, 0, -1.0f);
	glm::vec3 u = glm::vec3_up;
	glm::vec3 r = glm::vec3_right;

	// section dimensions
	float height = (float)tan(FOV * M_PI / 360.0f) * distance;
	float width = height * aspectRatio;

	// center point
	glm::vec3 c = glm::normalize(f) * distance;

	*it++ = c + (u * height) - (r * width); // top left point
	*it++ = c + (u * height) + (r * width); // top right point
	*it++ = c - (u * height) + (r * width); // bottom right point
	*it++ = c - (u * height) - (r * width); // bottom left point
}

// Compute Perspective Frustum
void Camera::ComputeFrustum()
{
	// Create Frustum 3D model
	vector<glm::vec3> positions(8);
	vector<glm::vec2> text_coord;
	vector<glm::vec3> normals(8);
	vector<unsigned short> indices;

	ComputePerspectiveSection(zNear, positions.begin());
	ComputePerspectiveSection(zFar, positions.begin() + 4);

	for (int i=0; i<8; i++) {
		normals.push_back(glm::vec3(0, 1, 0));
	}

	//PushQuad(indices, normals, 0, 1, 2, 3);
	//PushQuad(indices, normals, 7, 6, 5, 4);
	Utils3D::PushQuad(indices, 4, 5, 1, 0);
	Utils3D::PushQuad(indices, 6, 7, 3, 2);
	Utils3D::PushQuad(indices, 4, 0, 3, 7);
	Utils3D::PushQuad(indices, 5, 6, 2, 1);

	//frustum = new GameObject();
	//frustum->transform = transform;
	mesh = new Mesh();
	mesh->SetGlPrimitive(GL_QUADS);
	mesh->InitFromData(positions, normals, text_coord, indices);
	SetupAABB();
}

void Camera::SplitFrustum(unsigned int splits)
{
	ComputeFrustum();

	this->splits = splits;
	zones.clear();
	zones.reserve(splits);

	splitDistances.clear();
	splitDistances.resize(splits + 1);
	splitDistances[0] = zNear;
	vector<glm::vec3> planes((splits + 1) * 4);

	ComputePerspectiveSection(splitDistances[0], planes.begin());

	// Use logarithmic split scale
	float scale = log(zFar - zNear);

	for (unsigned int i = 1; i <= splits; i++) {
		splitDistances[i] = (float)pow(M_E, ((i * scale) / splits)) + zNear;
		ComputePerspectiveSection(splitDistances[i], planes.begin() + 4 * i);
	}

	///////////////////////////////////////////////////////////////////////////
	// Create AABB for each split section
	{
		vector<glm::vec2> text_coord;
		vector<glm::vec3> normals;
		vector<unsigned short> indices;

		for (int k = 0; k < 8; k++)
			normals.push_back(glm::vec3(0, 1, 0));

		Utils3D::PushQuad(indices, 4, 5, 1, 0);
		Utils3D::PushQuad(indices, 6, 7, 3, 2);
		Utils3D::PushQuad(indices, 4, 0, 3, 7);
		Utils3D::PushQuad(indices, 5, 6, 2, 1);

		// Create an AABB for each split section
		for (unsigned int i = 0; i < splits; i++) {
			GameObject *fz = new GameObject();
			fz->transform = transform;
			fz->mesh = new Mesh();

			for (int k = 0; k < 8; k++)
				fz->mesh->positions.push_back(planes[i * 4 + k]);

			fz->mesh->InitFromData(fz->mesh->positions, normals, text_coord, indices);
			fz->SetupAABB();

			zones.push_back(fz);
			Manager::Debug->Add(fz);
		}
	}
	///////////////////////////////////////////////////////////////////////////
}


// Compute bounding light space aligned AABB
void Camera::UpdateBoundingBox(DirectionalLight *Ref)
{
	// TODO - move this to DirectionalLight inclusive zones - because they are light space aligned so not related at all with the gameCamera, except camera properties
	for (unsigned int i = 0; i < splits; i++) {
		zones[i]->aabb->Update(Ref->transform->rotationQ);
		Transform *T = zones[i]->aabb->transform;
		float size = max(T->scale.x, T->scale.y);
		Ref->SetOrthgraphic(size, size, Ref->zNear, Ref->zFar);
		Ref->SetPosition(T->position - Ref->forward * Ref->distanceTo);
		Ref->lightViews[i] = Ref->View;
		Ref->lightProjections[i] = Ref->Projection;
	}

	aabb->Update(Ref->transform->rotationQ);
	Transform *T = aabb->transform;
	float size = max(T->scale.x, T->scale.z);
	Ref->SetOrthgraphic(size, size, Ref->zNear, Ref->zFar);
	Ref->SetPosition(T->position - Ref->forward * Ref->distanceTo);
}


void Camera::RenderDebug(const Shader *shader) const
{
	glLineWidth(3);
	glUniform4fv(shader->loc_debug_color, 1, glm::value_ptr(glm::color<glm::vec4>(255, 100, 65)));

	Manager::RenderSys->Set(RenderState::WIREFRAME, true);

		physicalDevice->Render(shader);
		Render(shader);
		for (auto zone : zones) {
			glLineWidth(1);
				zone->aabb->Render(shader);
			glLineWidth(3);
			glUniform4f(shader->loc_debug_color, 0.96f, 0.47f, 0.84f, 1);
			glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform->model));
			UtilsGPU::DrawPolygon(zone->mesh->positions);
		}

	glLineWidth(1);
	Manager::RenderSys->Revert(RenderState::WIREFRAME);

}

void Camera::BindProjectionDistances(const Shader *shader) const
{
	glUniform1f(shader->loc_z_far, zFar);
	glUniform1f(shader->loc_z_near, zNear);
}