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

namespace WorldControl
{
	enum class ControlMode {
		Local,
		World
	};

	enum class GizmoMode
	{
		Move,
		Rotate,
		Scale
	};

	#define GIZMO_MODE_ID(x) (static_cast<unsigned int>(x))
}

class DLLExport ColorPicking
	: public ObjectInput
	, public EventListener
{
	public:
		ColorPicking();
		~ColorPicking();

		void Init();
		void Update(Camera* activeCamera);

		void FocusCamera();
		void SetSelectedObject(GameObject *object);
		void ClearSelection();
		bool HasActiveSelection() const;
		GameObject* GetSelectedObject() const;
		void TriggerMouseSelection(int mouseX, int mouseY);

		void DrawSceneForPicking() const;

		void ToggleControlMode();
		void SetControlMode(WorldControl::ControlMode MODE);
		WorldControl::ControlMode GetControlMode() const;

		void SetGizmoMode(WorldControl::GizmoMode MODE);
		WorldControl::GizmoMode GetGizmoMode() const;

	private:
		void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods);
		void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods);
		void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY);
		void OnKeyPress(int key, int mod);

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

		WorldControl::GizmoMode gizmo_MODE;
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

		Camera *activeCamera;
		GameObject	*selectedObject;

		WorldControl::ControlMode controlMode;
};