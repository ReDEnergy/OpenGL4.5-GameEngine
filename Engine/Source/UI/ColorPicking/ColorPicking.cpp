#include "ColorPicking.h"

#include <iostream>
using namespace std;

#include <include/dll_export.h>
#include <include/gl_utils.h>
#include <include/glm.h>
#include <include/glm_utils.h>

#include <Editor/Gizmo.h>

#include <Core/GameObject.h>
#include <Core/Camera/Camera.h>
#include <Core/WindowManager.h>

#include <Component/AABB.h>
#include <Component/Mesh.h>
#include <Component/SkinnedMesh.h>
#include <Component/MeshRenderer.h>
#include <Component/Text.h>
#include <Component/Transform/Transform.h>
#include <Component/ObjectInput.h>
#include <Component/Renderer.h>
#ifdef PHYSICS_ENGINE
#include <Component/Physics.h>
#endif

#include <Debugging/TextureDebugger.h>

#include <Manager/Manager.h>
#include <Manager/ColorManager.h>
#include <Manager/TextureManager.h>
#include <Manager/ResourceManager.h>
#include <Manager/RenderingSystem.h>
#include <Manager/ShaderManager.h>
#include <Manager/SceneManager.h>
#include <Manager/EventSystem.h>

#include <Rendering/DirectOpenGL.h>

#include <Core/Engine.h>
#include <Core/WindowObject.h>
#include <Core/InputSystem.h>
#include <Core/Camera/Camera.h>

#include <GPU/FrameBuffer.h>
#include <GPU/Texture.h>
#include <GPU/Shader.h>

#include <Utils/OpenGL.h>
#include <Utils/GPU.h>

using namespace WorldControl;

struct _Gizmo
{
	GameObject *gizmo_object[3];
	glm::vec3 axis, color;
} gizmo[3];

GizmoMode events[3] = { GizmoMode::MOVE, GizmoMode::ROTATE, GizmoMode::SCALE };

ColorPicking::ColorPicking()
{
	controlMode = ControlMode::WORLD;
}

ColorPicking::~ColorPicking() {
}

void ColorPicking::Init()
{
	window = WindowManager::GetDefaultWindow();
	AttachTo(window);

	activeCamera = nullptr;
	selectedObject = nullptr;
	gizmo_isLocal = true;
	pickEvent = false; 
	focusEvent = false;
	gizmoEvent = false;
	
	focus_minSpeed = 1.0f;
	focus_maxSpeed = 5.0f;
	focus_accel = 0.25f;

	holdTransform = new Transform();

	gizmo_MODE = GizmoMode::MOVE;

	glm::ivec2 resolution = window->props.resolution;

	// Frame buffers
	// generate buffer for color picking
	FBO = new FrameBuffer();
	FBO->Generate(resolution.x, resolution.y, 1);

	//buffer for gizmo-only
	FBO_Gizmo = new FrameBuffer();
	FBO_Gizmo->Generate(resolution.x, resolution.y, 1);

	Manager::TextureDBG->SetChannel(1, FBO);

	// Get shaders
	cpShader = Manager::Shader->GetShader("simple");
	cpShaderSk = Manager::Shader->GetShader("simple-skinned");
	gizmoShader = cpShader;

	gizmoObject = new Gizmo();

	// TODO - maybe use only One Instance for rendering ?!
	// TODO - WHY X AND Z HAVE REVERSED COLORS ?!
	// gizmo struct init
	//x
	gizmo[0].color = glm::vec3(0, 0, 1);
	gizmo[0].axis = glm::vec3(1, 0, 0);

	//y
	gizmo[1].color = glm::vec3(0, 1, 0);
	gizmo[1].axis = glm::vec3(0, 1, 0);

	//z
	gizmo[2].color = glm::vec3(1, 0, 0);
	gizmo[2].axis = glm::vec3(0, 0, -1);

	//gizmo objects
	for (int i = 0; i <= 2; i++) {
		gizmo[i].gizmo_object[GIZMO_MODE_ID(GizmoMode::MOVE)] = Manager::Resource->GetGameObject("gizmo_move");
		gizmo[i].gizmo_object[GIZMO_MODE_ID(GizmoMode::ROTATE)] = Manager::Resource->GetGameObject("gizmo_rotate");
		gizmo[i].gizmo_object[GIZMO_MODE_ID(GizmoMode::SCALE)] = Manager::Resource->GetGameObject("gizmo_scale");
	}

	ResetGizmoRotation();
}

void ColorPicking::Update(Camera* activeCamera)
{
	this->activeCamera = activeCamera;

	if (pickEvent) {
		PickObject();
	}

	if (selectedObject)
	{
		UpdateGizmo();
		DrawGizmo();

		if (focusEvent) {
			FocusCamera();
		}
	}
}

void ColorPicking::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
		pickEvent = true;
		mousePosition = glm::ivec2(mouseX, mouseY);
	}
}

void ColorPicking::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	if (IS_BIT_SET(button, GLFW_MOUSE_BUTTON_LEFT)) {
		currentAxis = glm::vec3(0);
		gizmoEvent = false;
	}
}

void ColorPicking::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (!selectedObject)
		return;

	if (gizmoEvent)
	{
		switch (gizmo_MODE)
		{
		case GizmoMode::MOVE:
		{
			float dist = activeCamera->DistTo(*selectedObject);
			glm::vec3 delta((deltaX - deltaY) * dist / 500);

			auto axis = currentAxis;

			if (controlMode == ControlMode::LOCAL) {
				axis = selectedObject->transform->GetWorldRotation() * currentAxis;
			}

			auto newPos = selectedObject->transform->GetWorldPosition() + axis * delta;
			auto newCameraPos = activeCamera->transform->GetWorldPosition() + axis * delta;

			// Move Object
			selectedObject->transform->SetWorldPosition(newPos);

			// Move Camera if CTRL is pressed
			if (window->KeyHold(GLFW_KEY_LEFT_CONTROL))
			{
				activeCamera->transform->SetWorldPosition(newCameraPos);
			}
			break;
		}

		case GizmoMode::ROTATE:
		{
			glm::vec3 rotation_vector = currentAxis * glm::vec3(static_cast<float>(deltaX - deltaY));
			auto speed = selectedObject->transform->GetRotationSpeed();
			selectedObject->transform->SetRotationSpeed(0.3f);
			if (controlMode == ControlMode::WORLD)
			{
				if (rotation_vector.x) selectedObject->transform->RotateWorldOX(rotation_vector.x);
				if (rotation_vector.y) selectedObject->transform->RotateWorldOY(rotation_vector.y);
				if (rotation_vector.z) selectedObject->transform->RotateWorldOZ(rotation_vector.z);
			}
			else {
				if (rotation_vector.x) selectedObject->transform->RotateLocalOX(rotation_vector.x);
				if (rotation_vector.y) selectedObject->transform->RotateLocalOY(rotation_vector.y);
				if (rotation_vector.z) selectedObject->transform->RotateLocalOZ(rotation_vector.z);
			}
			selectedObject->transform->SetRotationSpeed(speed);
			break;
		}

		case GizmoMode::SCALE:
		{
			selectedObject->transform->SetScale(selectedObject->transform->GetScale() + currentAxis *
				glm::vec3(static_cast<float>(deltaX - deltaY)) / glm::vec3(200.0f));
			break;
		}
		default:
			break;
		}

		#ifdef PHYSICS_ENGINE
		if (selectedObject->transform->GetMotionState()) {
			if (selectedObject->physics)
				selectedObject->physics->SetTransform();
		}
		#endif

		// Update Gizmo
		UpdateGizmoPosition();
		if (controlMode == ControlMode::LOCAL && gizmo_MODE == GizmoMode::ROTATE) {
			UpdateGizmoRotation();
		}
	}
}

void ColorPicking::OnKeyPress(int key, int mod)
{
	if (!selectedObject) return;

	if (mod == GLFW_MOD_CONTROL)
	{
		GizmoMode prevEvent = gizmo_MODE;
		switch (key)
		{
		case GLFW_KEY_C: {
			holdTransform->Copy(*selectedObject->transform);
			cout << "[name]: " << selectedObject->GetName() << endl;
			cout << "[world pos]: " << selectedObject->transform->GetWorldPosition() << endl;
			cout << "[local pos]: " << selectedObject->transform->GetLocalPosition() << endl;
			cout << "[world euler]: " << selectedObject->transform->GetRotationEulerRad() * TO_DEGREES << endl;
			cout << "[local quat]: " << selectedObject->transform->GetRelativeRotation() << endl;
			cout << "[world quat]: " << selectedObject->transform->GetWorldRotation() << endl;
			cout << "[scale]: " << selectedObject->transform->GetScale() << endl;
			cout << endl;
			break;
		}
		case GLFW_KEY_D: {
			auto obj = new GameObject(*selectedObject);
			Manager::Scene->AddObject(obj);
			break;
		}
		case GLFW_KEY_DELETE: {
			Manager::Event->EmitAsync(EventType::EDITOR_OBJECT_REMOVED, selectedObject);
			Manager::Scene->RemoveObject(selectedObject, true);
			break;
		}
		case GLFW_KEY_V: {
			if (gizmo_MODE == GizmoMode::MOVE)
				selectedObject->transform->SetWorldPosition(holdTransform->GetWorldPosition());
			if (gizmo_MODE == GizmoMode::ROTATE)
				selectedObject->transform->SetWorldRotation(holdTransform->GetWorldRotation());
			break;
		}
		case GLFW_KEY_W: {
			gizmo_MODE = GizmoMode::MOVE;
			cout << "[world pos]: " << selectedObject->transform->GetWorldPosition() << endl;
			cout << "[local pos]: " << selectedObject->transform->GetLocalPosition() << endl;
			cout << endl;
			break;
		}
		case GLFW_KEY_E: {
			gizmo_MODE = GizmoMode::ROTATE;
			cout << "[world euler]: " << selectedObject->transform->GetRotationEulerRad() * TO_DEGREES << endl;
			cout << "[local quat]: " << selectedObject->transform->GetRelativeRotation() << endl;
			cout << "[world quat]: " << selectedObject->transform->GetWorldRotation() << endl;
			cout << endl;
			break;
		}
		case GLFW_KEY_R: {
			gizmo_MODE = GizmoMode::SCALE;
			cout << "[scale]: " << selectedObject->transform->GetScale() << endl << endl;
			break;
		}
		case GLFW_KEY_I: {
			selectedObject->LogDebugInfo();
			break;
		}
		case GLFW_KEY_L: {
			SetControlMode(controlMode == ControlMode::LOCAL ? ControlMode::WORLD : ControlMode::LOCAL);
			break;
		}
		case GLFW_KEY_F: {
			focus_currentSpeed = focus_minSpeed;
			focusEvent = true;
			break;
		}
		case GLFW_KEY_U: {
			if (selectedObject)
				selectedObject->transform->SetWorldRotation(glm::quat(1, 0, 0, 0));
			break;
		}
		default:
			break;
		}

		if (gizmo_MODE != prevEvent) {
			UpdateGizmo();
		}
	}
}

// TODO:
// Switch to TPS camera for viewing objects + distance to object (SCROLL)
// Move this code to Camera class - something like SetTarget() then FocusTarget() + provide DistToTarget()
void ColorPicking::FocusCamera()
{
	glm::vec3 scale = selectedObject->aabb->GetTransform()->GetScale();
	float dist = 3 * max(scale.x, max (scale.y, scale.z) );
	glm::vec3 position = selectedObject->transform->GetWorldPosition() + glm::normalize(activeCamera->transform->GetLocalOZVector()) * dist;

	glm::vec3 cameraPosition = activeCamera->transform->GetWorldPosition();
	
	if (glm::distance(cameraPosition, position) < 0.01f) {
		focusEvent = false;
		return;
	}

	focus_currentSpeed = min(focus_maxSpeed, focus_currentSpeed + focus_accel);
	activeCamera->SetPosition(cameraPosition + glm::normalize(position - cameraPosition) * min(focus_currentSpeed, glm::distance(position, cameraPosition)));
}

void ColorPicking::SetSelectedObject(GameObject * object)
{
	if (!object) {
		ClearSelection();
		return;
	}
		
	object->OnSelect();

	Manager::Event->EmitAsync(EventType::EDITOR_OBJECT_SELECTED, object);

	if (selectedObject == object) {
		return;
	}

	if (object) {
		selectedObject = object;
		// Align Gizmo to the current gameObj objects
		UpdateGizmo();
	}
}

GameObject* ColorPicking::GetSelectedObject() const
{
	return selectedObject;
}

void ColorPicking::TriggerMouseSelection(int mouseX, int mouseY)
{
	mousePosition = glm::ivec2(mouseX, mouseY);
	if (activeCamera)
		PickObject();
}

void ColorPicking::ClearSelection()
{
	selectedObject = nullptr;
	Manager::Event->EmitAsync(EventType::EDITOR_NO_SELECTION);
}

bool ColorPicking::HasActiveSelection() const
{
	return selectedObject != nullptr;
}

void ColorPicking::PickObject()
{
	if (selectedObject)
		UpdateGizmo(true);

	FBO->Bind();

	DrawSceneForPicking();

	// Read pixel from color-picking frame buffer
	unsigned char pickedColor[3];
	auto rez = FBO->GetResolution();
	mousePosition = (mousePosition * rez) / window->props.resolution;
	glReadPixels(mousePosition.x, rez.y - mousePosition.y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pickedColor);

	FrameBuffer::Unbind();

	glm::ivec3 color(pickedColor[0], pickedColor[1], pickedColor[2]);
	GetPickedObject(color);
	pickEvent = false;
}

void ColorPicking::GetPickedObject(const glm::ivec3 &colorID)
{
	GameObject *G = Manager::GetColor()->GetObjectByColor(colorID);

	// If gizmo was clicked
	gizmoEvent = false;
	for (int i = 0; i <= 2; i++) {
		if (gizmo[i].gizmo_object[GIZMO_MODE_ID(gizmo_MODE)] == G)
		{
			gizmoEvent = true;

			//the color matches the axis
			currentAxis = gizmo[i].color;
		}
	}

	if (!gizmoEvent) {
		SetSelectedObject(G);
	}

	CheckOpenGLError();
}

void ColorPicking::ToggleControlMode()
{
	SetControlMode(controlMode == ControlMode::WORLD ? ControlMode::LOCAL : ControlMode::WORLD);
}

void ColorPicking::SetControlMode(ControlMode mode)
{
	if (controlMode == mode) return;
	controlMode = mode;

	if (controlMode == ControlMode::WORLD) {
		cout << "Rotate: [WORLD]" << endl << endl;
	}
	else {
		cout << "Rotate: [LOCAL]" << endl << endl;
	}
	if (selectedObject)
		UpdateGizmo();
}

WorldControl::ControlMode ColorPicking::GetControlMode() const
{
	return controlMode;
}

void ColorPicking::SetGizmoMode(WorldControl::GizmoMode MODE)
{
	if (gizmo_MODE == MODE)
		return;

	gizmo_MODE = MODE;
	if (selectedObject)
		UpdateGizmo();
}

WorldControl::GizmoMode ColorPicking::GetGizmoMode() const
{
	return gizmo_MODE;
}

void ColorPicking::UpdateGizmo(bool picking)
{
	if (!activeCamera) return;

	float dist = activeCamera->DistTo(*selectedObject);
	glm::vec3 scale(dist);
	if (gizmo_MODE == GizmoMode::ROTATE) {
		scale.y = picking ? 2 * dist : dist / 4;
	}
	for (int i = 0; i < 3; i++) {
		gizmo[i].gizmo_object[GIZMO_MODE_ID(gizmo_MODE)]->transform->SetScale(scale);
		gizmo[i].gizmo_object[GIZMO_MODE_ID(gizmo_MODE)]->transform->SetWorldPosition(selectedObject->transform->GetWorldPosition());
	}
	gizmoObject->SetScale(scale);
	gizmoObject->transform->SetWorldPosition(selectedObject->transform->GetWorldPosition());

	UpdateGizmoRotation();
}

void ColorPicking::UpdateGizmoPosition()
{
	auto pos = selectedObject->transform->GetWorldPosition();
	for (int i = 0; i < 3; i++) {
		gizmo[i].gizmo_object[GIZMO_MODE_ID(gizmo_MODE)]->transform->SetWorldPosition(pos);
	}
	gizmoObject->transform->SetWorldPosition(pos);
}

void ColorPicking::UpdateGizmoRotation()
{
	auto offsetRotation = glm::quat();
	if (controlMode == ControlMode::LOCAL || gizmo_MODE == GizmoMode::SCALE) {
		offsetRotation = selectedObject->transform->GetWorldRotation();
	}

	//gizmoObject->transform->SetWorldRotation(offsetRotation); // TODO

	for (int i = 0; i < 3; i++) {
		gizmo[i].gizmo_object[GIZMO_MODE_ID(gizmo_MODE)]->transform->SetWorldRotation(offsetRotation * glm::quat(90.0f * gizmo[i].axis * TO_RADIANS));
	}
}

void ColorPicking::ResetGizmoRotation()
{
	for (int i = 0; i < 3; i++) {
		for (auto EVENT : events) {
			gizmo[i].gizmo_object[GIZMO_MODE_ID(EVENT)]->transform->SetWorldRotation(90.0f * gizmo[i].axis);
		}
	}
}

void ColorPicking::DrawSceneForPicking() const
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	cpShader->Use();
	activeCamera->BindViewProj(cpShader);

	auto renderingList = Manager::GetScene()->GetFrustrumObjects();

	for (auto *obj : renderingList) {
		if (obj->meshRenderer->mesh->meshType == MeshType::STATIC)
		{
			obj->RenderForPicking(cpShader);
		}
	}

	// Render Skinned meshes
	cpShaderSk->Use();
	activeCamera->BindViewProj(cpShaderSk);

	for (auto *obj : renderingList) {
		if (obj->meshRenderer->mesh->meshType == MeshType::SKINNED) {
			obj->RenderForPicking(cpShaderSk);
		}
	}

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	cpShader->Use();

	for (auto obj : renderingList) {
		if (obj->renderer->GetRenderingLayer() == RenderingLayer::ON_TOP)
			obj->RenderForPicking(cpShader);
	}

	glDepthMask(GL_TRUE);

	if (selectedObject) {
		RenderGizmoForPicking(cpShader);
	}

	glEnable(GL_DEPTH_TEST);
	CheckOpenGLError();
}

void ColorPicking::DrawGizmo() const
{
	FBO_Gizmo->Bind();
	gizmoShader->Use();

	activeCamera->BindPosition(gizmoShader->loc_eye_pos);
	activeCamera->BindViewMatrix(gizmoShader->loc_view_matrix);
	activeCamera->BindProjectionMatrix(gizmoShader->loc_projection_matrix);

	CheckOpenGLError();
	RenderGizmo(gizmoShader);
}

void ColorPicking::RenderGizmo(const Shader* shader) const
{
	glm::vec3 highlightColor = glm::vec3(1, 1, 0);

	RenderAxisHelpers(shader);

	if (gizmo_MODE == GizmoMode::ROTATE)
	{
		Manager::RenderSys->SetGlobalCulling(OpenGL::CULL::FRONT);
		GL_Utils::SetColorUnit(shader->loc_debug_color, 0.2f, 0.2f, 0.2f);
		for (int i = 0; i < 3; i++)
		{
			gizmo[i].gizmo_object[GIZMO_MODE_ID(gizmo_MODE)]->Render(shader);
		}
	}

	Manager::RenderSys->SetGlobalCulling(OpenGL::CULL::BACK);
	for (int i = 0; i < 3; i++)
	{
		GameObject *obj = gizmo[i].gizmo_object[GIZMO_MODE_ID(gizmo_MODE)];
		glm::vec3 color = (currentAxis == gizmo[i].color) ? highlightColor : gizmo[i].color;
		GL_Utils::SetColorUnit(shader->loc_debug_color, color);
		obj->Render(shader);
	}

	CheckOpenGLError();
	Manager::RenderSys->DisableGlobalCulling();
}

void ColorPicking::RenderGizmoForPicking(const Shader* shader) const
{
	Manager::RenderSys->SetGlobalCulling(OpenGL::CULL::NONE);
	for (int i = 0; i < 3; i++)
	{
		GameObject *obj = gizmo[i].gizmo_object[GIZMO_MODE_ID(gizmo_MODE)];
		GL_Utils::SetColorUnit(shader->loc_debug_color, gizmo[i].color);
		obj->RenderForPicking(shader);
	}
	Manager::RenderSys->DisableGlobalCulling();
}

void ColorPicking::RenderAxisHelpers(const Shader* shader) const
{
	glLineWidth(1);
	auto DirectGL = Manager::GetDirectGL();
	DirectGL->SetLineWidth(2);
	DirectGL->DrawStandardAxis(selectedObject->transform, shader);
}