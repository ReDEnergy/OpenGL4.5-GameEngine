#pragma once
#include <include/glm.h>
#include <include/dll_export.h>

#include <Component/ObjectInput.h>

class Camera;
class FrameBuffer;
class Gizmo;
class GameObject;
class ObjectInput;
class Shader;

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
		GameObject* GetSelectedObject() const;

	private:
		void GetPickedObject();
		void DrawSceneForPicking() const;

		void UpdateGizmo(bool picking = false);
		void UpdateGizmoPosition();
		void UpdateGizmoRotation();
		void UpdateGizmoScale();
		void ResetGizmoRotation();
		void DrawGizmo() const;
		void RenderGizmo(const Shader * shader, bool picking = false) const;

	public:
		FrameBuffer *FBO;
		FrameBuffer *FBO_Gizmo;

	private:
		Shader *cpShader;
		Shader *gizmoShader;

		glm::ivec2 mousePosition;

		GIZMO_EVENT GEVENT;
		Gizmo *gizmoObject;

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