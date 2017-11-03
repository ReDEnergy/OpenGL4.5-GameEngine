#pragma once
#include <include/dll_export.h>
#include <functional>

class GameObject;

class DLLExport GameScript
{
	friend class GameObject;
	
	public:
		GameScript();
		virtual ~GameScript();

		void OnUpdate(std::function<void()> updateFunc);

	protected:
		void AttachTo(GameObject *object);
		void Detach();
		virtual void Update() final;
		virtual void FrameUpdate() {};
		virtual void OnTriggerEnter(GameObject *object) {};
		virtual void OnTriggerExit(GameObject *object) {};

	protected:
		GameObject *GO;
		std::function<void()> _updateFunc;
		std::function<void(GameObject*)> _triggerEnter;
		std::function<void(GameObject*)> _triggerExit;
};