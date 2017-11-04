#pragma once
#include <atomic>

#include <Editor/Windows/Module/ModuleWindow.h>
#include <Editor/Interface/QtInput.h>
#include <Editor/include/QTUIForward.h>

#include <include/glm.h>
#include <Event/EventListener.h>
#include <GUI/Channels/ObjectPropertiesUIChannel.h>

class GameObject;

class ObjectPropertiesWindow : public ModuleWindow<ObjectPropertiesUIChannel>
{
	public:
		ObjectPropertiesWindow();
		virtual ~ObjectPropertiesWindow() {};

	private:
		void InitUI();
		void UpdateUI();
		void UpdateView(GameObject *ctx);
		void OnEvent(EventType Event, void *data);

	private:
		bool forceUpdate;
		std::atomic<bool> shouldUpdate;
		std::atomic<int> contextUpdate;
		GameObject *context;

		SimpleDropDown *meshType;
		SimpleTextBox *objectName;
		GLMVecComponent<glm::vec3> *limitAxis;
		GLMVecComponent<glm::vec3> *worldPosition;
		GLMVecComponent<glm::vec3> *localPosition;
		GLMVecComponent<glm::vec3> *worldEuler;
		GLMVecComponent<glm::vec3> *localEuler;
		GLMVecComponent<glm::vec3> *worldScale;
		GLMVecComponent<glm::quat> *worldQuat;
		GLMVecComponent<glm::quat> *localQuat;

		#ifdef PHYSICS_ENGINE
		QPushButton *addPhysicsBtn;
		QPushButton *removePhysicsBtn;
		SimpleCheckBox *isDyanmic;
		SimpleCheckBox *hasGravity;
		SimpleCheckBox *isTrigger;
		SimpleCheckBox *isKinematic;
		SimpleFloatInput *objectMass;
#endif
};