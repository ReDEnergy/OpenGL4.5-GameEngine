#pragma once
#include <include/glm.h>
#include <include/dll_export.h>

#include <Component/ObjectInput.h>
#include <Event/EventListener.h>

class Camera;
class FrameBuffer;
class Gizmo;
class GameObject;
class ObjectInput;
class Shader;
class Transform;

namespace ENUM_GIZMO_EVENT {
	enum GE {
		MOVE,
		ROTATE,
		SCALE,
		ROTATE_LOCAL,
		ROTATE_WORLD
	};
}
typedef ENUM_GIZMO_EVENT::GE GIZMO_EVENT;

class DLLExport ColorPicking
	: public ObjectInput
	, public EventListener
{
	public:
		ColorPicking();
		~ColorPicking();

		void Init();
		void Update(const Camera* activeCamera);
		void OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods);
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY);
		void OnKeyPress(int key, int mod);

		void FocusCamera();
		void SetSelectedObject(GameObject *object);
		void ClearSelection();
		bool HasActiveSelection() const;
		GameObject* GetSelectedObject() const;

		void DrawSceneForPicking() const;

		void OnEvent(EventType Event, void *data);

	private:
		void PickObject();
		void GetPickedObject(const glm::ivec3 &colorID);

		void UpdateGizmo(bool picking = false);
		void UpdateGizmoPosition();
		void UpdateGizmoRotation();
		void ResetGizmoRotation();
		void DrawGizmo() const;
		void RenderGizmo(const Shader * shader) const;
		void RenderGizmoForPicking(const Shader * shader) const;
		void RenderAxisHelpers(const Shader * shader) const;

	public:
		FrameBuffer *FBO;
		FrameBuffer *FBO_Gizmo;

	private:
		Shader *cpShader;
		Shader *cpShaderSk;
		Shader *gizmoShader;

		glm::ivec2 mousePosition;

		GIZMO_EVENT GEVENT;
		Gizmo *gizmoObject;
		Transform *holdTransform;

		bool pickEvent;
		bool gizmoEvent;
		bool gizmo_isLocal;
		bool focusEvent;

		float focus_currentSpeed;
		float focus_minSpeed;
		float focus_maxSpeed;
		float focus_accel;

		glm::vec3 currentAxis;

		const Camera *activeCamera;
		GameObject	*selectedObject;
		GIZMO_EVENT rotateMode;
};