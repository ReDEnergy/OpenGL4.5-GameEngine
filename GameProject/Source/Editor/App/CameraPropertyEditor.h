#pragma once
#include <Editor/Windows/Interface/DockWindow.h>
#include <Editor/Interface/QtInput.h>

#include <include/glm.h>
#include <Event/EventListener.h>

class CameraPropertyEditor
	: public DockWindow
	, public EventListener
{
	public:
		CameraPropertyEditor();
		virtual ~CameraPropertyEditor() {};

	private:
		void InitUI();
		void Update();
		void OnEvent(EventType Event, void *data);

	private:
		GLMVecComponent<glm::vec3> *cameraPosition;
		GLMVecComponent<glm::vec3> *cameraRotation;
		SimpleFloatInput *camerFoV;
		SimpleFloatInput *moveSpeed;

		bool forceUpdate;
};