#include "Gizmo.h"

#include <Manager/Manager.h>
#include <Manager/ResourceManager.h>
#include <Component/Transform/Transform.h>
#include <Component/Mesh.h>
#include <GPU/Shader.h>

using namespace std;

Gizmo::Gizmo()
	: GameObject("gizmo")
{
	// Config X axis
	GameObject* GO = Manager::GetResource()->GetGameObject("gizmo_rotate");
	AddChild(GO);

	// Config Y axis
	GO = Manager::GetResource()->GetGameObject("gizmo_rotate");
	GO->transform->SetWorldRotation(glm::vec3(0, 0, 90));
	AddChild(GO);

	// Config Z axis
	GO = Manager::GetResource()->GetGameObject("gizmo_rotate");
	GO->transform->SetWorldRotation(glm::vec3(90, 0, 0));
	AddChild(GO);
}

Gizmo::~Gizmo()
{
}

void Gizmo::SetScale(glm::vec3 scale)
{
	for (auto child : _children) {
		child->transform->SetScale(scale);
	}
}

void Gizmo::SetMode(GIZMO_MODE mode)
{
}

