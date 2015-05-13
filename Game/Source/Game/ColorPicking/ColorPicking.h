#pragma once

#include <Component/ObjectInput.h>

#include <include/glm.h>

class Camera;
class GameObject;
class FrameBuffer;
class ObjectInput;
class Shader;

enum Gizmo_Action{
	Move=0,
	Rotate=1,
	Scale=2
};

class ColorPicking : public ObjectInput {

public:
	ColorPicking();
	~ColorPicking();

	void Init();
	void Update(const Camera* activeCamera);
	void OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods);
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY);
	void OnKeyPress(int key, int mod);
	void OnKeyRelease(int key, int mod);
	void DrawGizmo();

	FrameBuffer *FBO;
	FrameBuffer *FBO_Gizmo;

	GameObject	*selectedObject;

private:
	Shader *cpShader;
	Shader *gizmoShader;

	glm::ivec2 mousePosition;
	bool pickEvent;
	bool gizmoEvent;
	bool gizmo_isLocal;
	bool gizmo_moveCameraAlong;

	glm::vec3 gizmoPosition;
	glm::vec3 gizmoLocalRotation;
	glm::vec3 currentAxis;

	const Camera *camera;

	enum Gizmo_Action gizmo_action;
};