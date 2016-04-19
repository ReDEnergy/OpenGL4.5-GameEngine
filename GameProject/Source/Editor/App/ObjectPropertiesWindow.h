#pragma once
#include <Editor/Windows/Interface/DockWindow.h>
#include <Editor/Interface/QtInput.h>

#include <include/glm.h>
#include <Event/EventListener.h>

class GameObject;
class SimpleTextBox;

class ObjectPropertiesWindow
	: public DockWindow
	, public EventListener
{
	public:
		ObjectPropertiesWindow();
		virtual ~ObjectPropertiesWindow() {};

	private:
		void InitUI();
		void Update();
		void ForceUpdate();
		void OnEvent(EventType Event, void *data);

	private:
		bool forceUpdate;
		GameObject *context;

		GLMVecComponent<glm::vec3> *worldPosition;
		GLMVecComponent<glm::vec3> *worldEuler;
		GLMVecComponent<glm::vec3> *worldScale;
		GLMVecComponent<glm::quat> *worldQuat;
		SimpleDropDown *meshType;
		SimpleTextBox *objectName;
};