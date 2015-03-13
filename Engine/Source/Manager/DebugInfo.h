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
		void Add(GameObject *obj);
		void Remove(GameObject *obj);
		void InitManager(const char *info);
		void Render(const Camera *camera) const;
		void BindForRendering(const Camera *camera) const;

	public:
		bool debugView;
		bool debugMessages;
		FrameBuffer *FBO;
		std::list<GameObject*> objects;
};