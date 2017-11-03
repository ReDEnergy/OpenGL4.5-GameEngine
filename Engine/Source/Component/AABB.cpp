#include "AABB.h"

#include <vector>

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

using namespace std;

static GameObject* box = nullptr;

vector<glm::vec3> BBOX =
{
	glm::vec3( 1,  1,  1),
	glm::vec3( 1,  1, -1),
	glm::vec3( 1, -1,  1),
	glm::vec3( 1, -1, -1),

	glm::vec3(-1,  1,  1),
	glm::vec3(-1,  1, -1),
	glm::vec3(-1, -1,  1),
	glm::vec3(-1, -1, -1)
};

inline void GetBoundingLimits(const glm::vec3 &point, glm::vec3 &limitMin, glm::vec3 &limitMax)
{
	if (point.x > limitMax.x)	limitMax.x = point.x;
	if (point.y > limitMax.y)	limitMax.y = point.y;
	if (point.z > limitMax.z)	limitMax.z = point.z;

	if (point.x < limitMin.x)	limitMin.x = point.x;
	if (point.y < limitMin.y)	limitMin.y = point.y;
	if (point.z < limitMin.z)	limitMin.z = point.z;
}

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

void AABB::Update(const glm::quat &rotationQ)
{
	auto rotation = gameObj->transform->GetWorldRotation();
	auto position = gameObj->transform->GetWorldPosition();
	auto scale = gameObj->transform->GetScale();

	auto mesh = gameObj->meshRenderer->mesh;

	auto objHalfSize = mesh->GetHalfSize() * scale;
	auto meshCenter = mesh->GetCenterPoint();

	auto q = glm::inverse(rotationQ) * rotation;

	glm::vec3 minValue(0), maxValue(0);
	for (auto point : BBOX) {
		point *= objHalfSize;
		auto pt = glm::rotate(q, point);
		GetBoundingLimits(pt, minValue, maxValue);
	}

	halfSize = (maxValue - minValue) / 2.0f;
	auto centerOffset = position - meshCenter;
	centerOffset = glm::rotate(rotation, meshCenter * scale);

	transform->SetWorldPosition(centerOffset + position);
	transform->SetWorldRotationAndScale(rotationQ, halfSize * 2.0f);

	UpdateChildren(rotationQ);
}

void AABB::ComputeLocal()
{
	auto rotation = gameObj->transform->GetWorldRotation();
	auto position = gameObj->transform->GetWorldPosition();
	auto scale = gameObj->transform->GetScale();
	auto mesh = gameObj->GetMesh();

	auto objSize = mesh->GetHalfSize() * scale * 2.0f;
	auto meshCenter = mesh->GetCenterPoint();
	auto offset = glm::rotate(rotation, meshCenter * scale);

	transform->SetScale(objSize);
	transform->SetWorldPosition(offset + position);
	transform->SetWorldRotation(rotation);

	for (auto obj : gameObj->_children) {
		if (obj->aabb)
			obj->aabb->ComputeLocal();
	}
}

const Transform * AABB::GetTransform() const
{
	return transform;
}

void AABB::Render(const Shader *shader) const
{
	box->transform = transform;
	box->Render(shader);
	for (auto obj : gameObj->_children) {
		if (obj->aabb)
			obj->aabb->Render(shader);
	}
}

void AABB::RenderObjectBoundingBox(const Shader *shader) const
{
	box->transform = transform;
	box->Render(shader);
	for (auto obj : gameObj->_children) {
		if (obj->aabb)
			obj->aabb->RenderObjectBoundingBox(shader);
	}
}

