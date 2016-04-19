#include "AABB.h"

#include <include/gl_utils.h>
#include <include/glm_utils.h>

#include <Component/Transform/Transform.h>
#include <Component/Mesh.h>
#include <Component/MeshRenderer.h>
#include <Core/Camera/Camera.h>
#include <Core/GameObject.h>
#include <GPU/Shader.h>
#include <Manager/Manager.h>
#include <Manager/ResourceManager.h>
#include <Utils/3D.h>

static GameObject* box = nullptr;

AABB::AABB(GameObject *obj)
	: gameObj(obj)
{
	transform = new Transform();
	//if (obj->mesh == nullptr) {
	//	cout << "file: " << __FILE__ << "line: " << __LINE__ << endl;
	//	assert(false);
	//}
}

AABB::~AABB() {
	SAFE_FREE(transform);
}

// TODO - move this to a cached pool of control objects (in the resource manager)
// Actually, a better idea would be to use render all bounding boxes through OpenGL instantiation

void AABB::Init() {
	box = Manager::Resource->GetGameObject("box");
}

// Collision test not exactly ok
// probably different rotation are used for halfSize and positions

bool AABB::Overlaps(AABB *aabb)
{
	//auto posA = glm::rotate(glm::inverse(aabb->transform->rotationQ), aabb->transform->position);
	//auto posB = glm::rotate(glm::inverse(transform->rotationQ), transform->position);
	//auto delta = posA - posB;
	//auto half = halfSize + aabb->halfSize;

	auto q = glm::inverse(transform->GetWorldRotation());
	auto deltaPos = transform->GetWorldPosition() - aabb->transform->GetWorldPosition();
	auto delta = glm::rotate(q, deltaPos);
	auto half = halfSize + aabb->halfSize;

	bool overlapsX = abs(delta.x) < half.x;
	bool overlapsY =     delta.y  < half.y;
	bool overlapsZ = abs(delta.z) < half.z;
	return overlapsX && overlapsY && overlapsZ;
}

void AABB::UpdateChildren(const glm::quat & rotationQ)
{
	for (auto obj : gameObj->_children) {
		if (obj->aabb)
			obj->aabb->Update(rotationQ);
	}
}

void AABB::Update(glm::quat rotationQ)
{
	auto q = glm::inverse(rotationQ) * gameObj->transform->GetWorldRotation();
	auto mesh = gameObj->meshRenderer->mesh;

	glm::vec3 minValue, maxValue = glm::rotate(q, mesh->bbox->points[0]);

	for (auto point: mesh->bbox->points) {

		point = glm::rotate(q, point);

		// find max
		if (point.x > maxValue.x)
			maxValue.x = point.x;
		if (point.y > maxValue.y)
			maxValue.y = point.y;
		if (point.z > maxValue.z)
			maxValue.z = point.z;

		// find min
		if (point.x < minValue.x)
			minValue.x = point.x;
		if (point.y < minValue.y)
			minValue.y = point.y;
		if (point.z < minValue.z)
			minValue.z = point.z;
	}

	halfSize = (maxValue - minValue) / 2.0f * gameObj->transform->GetScale();
	center = (maxValue + minValue) / 2.0f * gameObj->transform->GetScale();
	center = glm::rotate(rotationQ, center);

	transform->SetScale(halfSize * 2.0f);
	transform->SetWorldPosition(center + gameObj->transform->GetWorldPosition());
	transform->SetWorldRotation(rotationQ);

	UpdateChildren(rotationQ);
}

void AABB::Render(const Shader *shader) const
{
	GL_Utils::SetColorUnit(shader->loc_debug_color, gameObj->GetColorID());
	box->transform = transform;
	box->Render(shader);
	for (auto obj : gameObj->_children) {
		if (obj->aabb)
			obj->aabb->Render(shader);
	}
}
