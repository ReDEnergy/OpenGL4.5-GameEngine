#pragma once
#include <list>

#include <include/dll_export.h>

class GameObject;
class FrameBuffer;
class Camera;

class DLLExport DebugInfo
{
	public:
		enum class BBOX_MODE
		{
			CAMERA_SPACE,
			OBJECT_SAPCE,
			LIGHT_SPACE
		};

	protected:
		DebugInfo();
		~DebugInfo();

	public:
		void Init();
		void InitManager(const char *info);

		void Add(GameObject *obj);
		void Remove(GameObject *obj);
		bool Toggle();

		void SetBoundingBoxMode(BBOX_MODE mode);

		bool GetActiveState() const;
		BBOX_MODE GetBoundingBoxMode() const;

		void Render(const Camera *camera) const;

	public:
		FrameBuffer *FBO;

	private:
		bool debugView;
		BBOX_MODE bboxMode;
		std::list<GameObject*> objects;
};