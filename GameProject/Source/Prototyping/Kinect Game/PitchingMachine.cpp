#include "pch.h"
#include "PitchingMachine.h"

#include <Game/Actors/Physics/PhysicsBall.h>
#include <Game/Actors/Physics/KinematicBall.h>

PitchingMachine::PitchingMachine()
{
	source = Manager::GetResource()->GetGameObject("sphere");
	source->transform->SetWorldPosition(glm::vec3(0, 5, 0));
	Manager::GetScene()->AddObject(source);
}

void PitchingMachine::Init()
{

}

void PitchingMachine::Update()
{
}

void PitchingMachine::PitchNewBall()
{
	auto pos = source->transform->GetWorldPosition();
	auto dir = source->transform->GetLocalOZVector();
	GameObject *obj = new PhysicsBall(0.25f, pos, dir);
	Manager::GetScene()->AddObject(obj);
}

void PitchingMachine::NewKinematicBall()
{
	GameObject *obj = new KinematicBall(1.0f);
	Manager::GetScene()->AddObject(obj);
}

void PitchingMachine::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_N) {
		PitchNewBall();
	}
	if (key == GLFW_KEY_K) {
		NewKinematicBall();
	}
}

void PitchingMachine::OnEvent(EventType Event, void * data)
{
}
