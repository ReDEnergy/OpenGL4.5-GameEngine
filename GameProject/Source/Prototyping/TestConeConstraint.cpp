#include "pch.h"
#include "TestConeConstraint.h"
#include <SkeletalSystem/SkeletalJoint.h>

TestConeConstraint::TestConeConstraint()
{
		
	ROOT = new SkeletalJoint("ROOT-TEST");
	auto child1 = new SkeletalJoint("child1");
	auto child2 = new SkeletalJoint("child2");

	ROOT->AddChild(child1);
	ROOT->AddChild(child2);

	ROOT->transform->SetWorldPosition(glm::vec3(2, 2, 0));
	child1->transform->SetLocalPosition(glm::vec3(0.2, 0.3, 0));
	child2->transform->SetLocalPosition(glm::vec3(-.3, 0.4, 0.15));

	Manager::GetScene()->AddObject(ROOT);

}

void TestConeConstraint::Init()
{

}

void TestConeConstraint::Render(Camera * camera)
{
	OpenGL::DrawCircle(glm::vec3(1), 2, 32);
}

void TestConeConstraint::Update(float deltaTime)
{
}

void TestConeConstraint::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_N) {

		auto wr = ROOT->transform->GetWorldRotation();
		auto inv = glm::inverse(wr) * wr;
		ROOT->transform->SetWorldRotation(inv);
	}
}