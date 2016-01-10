#pragma once
#include "pch.h"

#include <unordered_map>

using namespace std;

///////////////////////////////////////////////////////////////////////////////
// THIS CLASS DEFINITION SERVES AS A TEMPLATE FOR EASY PROTOTYPING
// INTENDED TO BE USED AS A BASE CLASS
// ONLY GENERAL FUNCTIONALITIES MAY BE IMPLEMENTED HERE

class TestConeConstraint :
	public Prototype
{
	public:
		TestConeConstraint();
		~TestConeConstraint() {};

		void Init();
		void Render(Camera *camera);
		void Update(float deltaTime);

		// Listen for mouse and keyboard events
		void Update(float deltaTime, int mods) {};
		void OnKeyPress(int key, int mods);
		void OnKeyRelease(int key, int mods) {};
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {};
		void OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods) {};

		// Listen for Events
		void OnEvent(const string& eventID, void *data) const {};
		void OnEvent(EventType Event, void *data) const {};

	private:
		SkeletalJoint *ROOT;
};