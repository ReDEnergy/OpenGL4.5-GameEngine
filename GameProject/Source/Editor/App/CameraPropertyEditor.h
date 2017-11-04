#pragma once
#include <Editor/Interface/QtInput.h>
#include <Editor/Windows/Module/ModuleWindow.h>

#include <Event/EventListener.h>

class Camera;
class SimpleTextBox;
class SimpleFloatInput;

class CameraPropertyEditor : public ModuleWindow<>
{
	public:
		CameraPropertyEditor();
		virtual ~CameraPropertyEditor() {};

	private:
		void InitUI();
		void InitUIState();
		void UpdateUI();
		void OnEvent(EventType Event, void *data);

	private:
		GLMVecComponent<glm::vec3> *cameraPosition;
		GLMVecComponent<glm::vec3> *cameraRotation;
		SimpleFloatInput *camerFoV;
		SimpleFloatInput *moveSpeed;
		SimpleTextBox *cameraName;
		Camera *activeCamera;

		std::atomic<bool> shouldUpdate;
		bool updateNewCamera;
};