//#include <pch.h>
#include "AABB.h"

#include <include/glm_utils.h>

#include <Component/Transform.h>
#include <Component/Mesh.h>
#include <Core/Camera/Camera.h>
#include <Core/GameObject.h>
#include <GPU/Shader.h>
#include <Manager/Manager.h>
#include <Manager/ResourceManager.h>
#include <Utils/3D.h>

static GameObject* box = nullptr;

AABB::AABB(GameObject *obj)
	: obj(obj)
{
	transform = new Transform();

	if (obj->mesh == nullptr) {
		cout << "file: " << __FILE__ << "line: " << __LINE__ << endl;
		assert(false);
	}
}

AABB::~AABB() {
}

void AABB::Init() {
	box = Manager::Resource->GetGameObject("box");
}

// Collision test not exactly ok
// prbably different rotation are used for halfSize and positions

bool AABB::Overlaps(AABB *aabb)
{
	auto q = glm::inverse(transform->rotationQ);
	auto deltaPos = transform->position - aabb->transform->position;
	auto delta = glm::rotate(q, deltaPos);
	auto half = halfSize + aabb->halfSize;
	bool overlapsX = abs(delta.x) < half.x;
	bool overlapsY =     delta.y  < half.y;
	bool overlapsZ = abs(delta.z) < half.z;
	return overlapsX && overlapsY && overlapsZ;
}

void AABB::Update()
{
	Update(glm::quat(1.0f, 0, 0, 0));
}

void AABB::Update(glm::quat rotationQ)
{
	auto q = glm::inverse(rotationQ) * obj->transform->rotationQ;

	glm::vec3 min, max = glm::rotate(q, obj->mesh->bbox->points[0]);

	for (auto point: obj->mesh->bbox->points) {

		point = glm::rotate(q, point);

		// find max
		if (point.x > max.x)
			max.x = point.x;
		if (point.y > max.y)
			max.y = point.y;
		if (point.z > max.z)
			max.z = point.z;

		// find min
		if (point.x < min.x)
			min.x = point.x;
		if (point.y < min.y)
			min.y = point.y;
		if (point.z < min.z)
			min.z = point.z;
	}

	halfSize = (max - min) / 2.0f * obj->transform->scale;
	center = (max + min) / 2.0f * obj->transform->scale;
	center = glm::rotate(rotationQ, center);

	transform->scale = halfSize * 2.0f;
	transform->position = center + obj->transform->position;
	transform->SetRotation(rotationQ);
}

void AABB::Render(const Shader *shader) const
{
	glUniform4fv(shader->loc_debug_color, 1, glm::value_ptr(obj->colorID));
	box->transform = transform;
	box->Render(shader);
}