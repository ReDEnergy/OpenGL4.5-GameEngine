#pragma once
#include "pch.h"

using namespace std;

class PitchingMachine :
	public Prototype
{
	public:
		PitchingMachine();
		~PitchingMachine() {};

		void Init();

	private:
		void Update();
		void PitchNewBall();

		void NewKinematicBall();

		// Listen for mouse and keyboard events
		void OnKeyPress(int key, int mods);
		void OnKeyRelease(int key, int mods) {};
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {};
		void OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods) {};

		// Listen for Events
		void OnEvent(const string& eventID, void *data) {};
		void OnEvent(EventType Event, void *data);

	private:
		GameObject *source;

};