#pragma once
#include <include/dll_export.h>
#include <Event/EventListener.h>
#include <Component/ObjectInput.h>

class Camera;
class Shader;

///////////////////////////////////////////////////////////////////////////////
// THIS CLASS DEFINITION SERVES AS A TEMPLATE FOR PROTOTYPING
// INTENDED TO BE USED AS A BASE CLASS
// ONLY GENERAL FUNCTIONALITIES MAY BE ADDED HERE

class DLLExport Prototype :
	public ObjectInput,
	public EventListener
{
	public:
		Prototype() {};
		~Prototype() {};

		void Init() {};
		void Render(Camera *camera) const {};
		void Update(float deltaTime) {};

		// Listen for mouse and keyboard events
		void OnInputUpdate(float deltaTime, int mods) {};
		void OnKeyPress(int key, int mods) {};
		void OnKeyRelease(int key, int mods) {};
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {};
		void OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods) {};

	protected:
		// Listen for Events
		void OnEvent(const std::string& eventID, void *data) {};
		void OnEvent(EventType Event, void *data) {};
};