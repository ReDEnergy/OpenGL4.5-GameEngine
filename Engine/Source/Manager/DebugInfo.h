#pragma once
#include <list>

#include <include/dll_export.h>

class GameObject;
class FrameBuffer;
class Camera;

class DLLExport DebugInfo
{
	protected:
		DebugInfo();
		~DebugInfo();

	public:
		void Init();
		void InitManager(const char *info);

		void Add(GameObject *obj);
		void Remove(GameObject *obj);
		void Update(const Camera *camera);
		bool Toggle();

		bool GetActiveState() const;

	private:
		void Render(const Camera *camera) const;

	public:
		FrameBuffer *FBO;

	private:
		bool debugView;
		std::list<GameObject*> objects;
};