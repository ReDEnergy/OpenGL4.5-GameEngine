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


struct _Gizmo {
	GameObject *gizmo_object[3];
	glm::vec3 axis, color;
} gizmo[3];

GIZMO_EVENT events[3] = { GIZMO_EVENT::MOVE, GIZMO_EVENT::ROTATE, GIZMO_EVENT::SCALE };

ColorPicking::ColorPicking()
{
	rotateMode = GIZMO_EVENT::ROTATE_WORLD;
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

	GEVENT = GIZMO_EVENT::MOVE;

	glm::ivec2 resolution = Engine::Window->props.resolution;

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
		gizmo[i].gizmo_object[GIZMO_EVENT::MOVE] = Manager::Resource->GetGameObject("gizmo_move");
		gizmo[i].gizmo_object[GIZMO_EVENT::ROTATE] = Manager::Resource->GetGameObject("gizmo_rotate");
		gizmo[i].gizmo_object[GIZMO_EVENT::SCALE] = Manager::Resource->GetGameObject("gizmo_scale");
	}

	ResetGizmoRotation();
}

void ColorPicking::Update(const Camera* activeCamera)
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

void ColorPicking::OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods)
{
	if (button == MOUSE_BUTTON::LEFT && action == GLFW_PRESS) {
		mousePosition = glm::ivec2(mouseX, mouseY);
		pickEvent = true;
	}

	if (button == MOUSE_BUTTON::LEFT && action == GLFW_RELEASE) {
		currentAxis = glm::vec3(0, 0, 0);
		gizmoEvent = false;
	}
}

void ColorPicking::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	if (!selectedObject)
		return;

	if (gizmoEvent)
	{
		switch (GEVENT)
		{
		case GIZMO_EVENT::MOVE:
		{
			float dist = activeCamera->DistTo(selectedObject);
			glm::vec3 delta((deltaX - deltaY) * dist / 500);

			// Move Object
			selectedObject->transform->SetWorldPosition(selectedObject->transform->GetWorldPosition() + currentAxis * delta);

			// Move Camera if CTRL is pressed
			if (window->KeyHold(GLFW_KEY_LEFT_CONTROL))
			{
				activeCamera->transform->SetWorldPosition(activeCamera->transform->GetWorldPosition() + currentAxis * delta);
			}
			break;
		}

		case GIZMO_EVENT::ROTATE:
		{
			glm::vec3 rotation_vector = currentAxis * glm::vec3(static_cast<float>(deltaX - deltaY));
			auto speed = selectedObject->transform->GetRotationSpeed();
			selectedObject->transform->SetRotationSpeed(0.3f);
			if (rotateMode == GIZMO_EVENT::ROTATE_WORLD)
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

		case GIZMO_EVENT::SCALE:
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
				selectedObject->physics->UpdatePhysicsEngine();
		}
		#endif

		// Update Gizmo
		UpdateGizmoPosition();
		if (rotateMode == GIZMO_EVENT::ROTATE_LOCAL) {
			UpdateGizmoRotation();
		}
	}
}

void ColorPicking::OnKeyPress(int key, int mod)
{
	if (!selectedObject) return;

	if (mod == GLFW_MOD_CONTROL)
	{
		GIZMO_EVENT prevEvent = GEVENT;
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
			if (GEVENT == GIZMO_EVENT::MOVE)
				selectedObject->transform->SetWorldPosition(holdTransform->GetWorldPosition());
			if (GEVENT == GIZMO_EVENT::ROTATE)
				selectedObject->transform->SetWorldRotation(holdTransform->GetWorldRotation());
			break;
		}
		case GLFW_KEY_W: {
			GEVENT = GIZMO_EVENT::MOVE;
			cout << "[world pos]: " << selectedObject->transform->GetWorldPosition() << endl;
			cout << "[local pos]: " << selectedObject->transform->GetLocalPosition() << endl;
			cout << endl;
			break;
		}
		case GLFW_KEY_E: {
			GEVENT = GIZMO_EVENT::ROTATE;
			cout << "[world euler]: " << selectedObject->transform->GetRotationEulerRad() * TO_DEGREES << endl;
			cout << "[local quat]: " << selectedObject->transform->GetRelativeRotation() << endl;
			cout << "[world quat]: " << selectedObject->transform->GetWorldRotation() << endl;
			cout << endl;
			break;
		}
		case GLFW_KEY_R: {
			GEVENT = GIZMO_EVENT::SCALE;
			cout << "[scale]: " << selectedObject->transform->GetScale() << endl;
			cout << endl;
			break;
		}
		case GLFW_KEY_I: {
			selectedObject->LogDebugInfo();
			break;
		}
		case GLFW_KEY_L: {
			if (rotateMode == GIZMO_EVENT::ROTATE_LOCAL) {
				rotateMode = GIZMO_EVENT::ROTATE_WORLD;
				ResetGizmoRotation();
				cout << "Rotate: [WORLD]" << endl;
				cout << endl;
			}
			else {
				rotateMode = GIZMO_EVENT::ROTATE_LOCAL;
				cout << "Rotate: [LOCAL]" << endl;
				cout << endl;
			}
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

		if (GEVENT != prevEvent) {
			UpdateGizmo();
		}
	}
}

// TODO:
// Switch to TPS camera for viewing objects + distance to object (SCROLL)

void ColorPicking::FocusCamera()
{
	glm::vec3 scale = selectedObject->aabb->transform->GetScale();
	float dist = 3 * max(scale.x, max (scale.y, scale.z) );
	glm::vec3 position = selectedObject->transform->GetWorldPosition() + glm::normalize(activeCamera->transform->GetLocalOZVector()) * dist;

	glm::vec3 cameraPosition = activeCamera->transform->GetWorldPosition();
	
	if (glm::distance(cameraPosition, position) < 0.01) {
		focusEvent = false;
		return;
	}

	focus_currentSpeed = min(focus_maxSpeed, focus_currentSpeed + focus_accel);
	activeCamera->transform->SetWorldPosition(cameraPosition + glm::normalize(position - cameraPosition) * min(focus_currentSpeed, glm::distance(position, cameraPosition)));
	const_cast<Camera*>(activeCamera)->Update();
}

void ColorPicking::SetSelectedObject(GameObject * object)
{
	if (object)
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

	// Read pixel from colorpicking frame buffer
	unsigned char pickedColor[3];
	glReadPixels(mousePosition.x, Engine::Window->props.resolution.y - mousePosition.y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pickedColor);
	FrameBuffer::Unbind(window);

	glm::ivec3 color(pickedColor[0], pickedColor[1], pickedColor[2]);
	GetPickedObject(color);
	pickEvent = false;
}

void ColorPicking::GetPickedObject(const glm::ivec3 &colorID)
{
	GameObject *G = Manager::GetColor()->GetObjectByColor(colorID);

	// check if gizmo was clicked
	gizmoEvent = false;
	for (int i = 0; i <= 2; i++) {
		if (gizmo[i].gizmo_object[GEVENT] == G)
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

void ColorPicking::OnEvent(EventType Event, void * data)
{
	if (Event == EventType::EDITOR_OBJECT_DELETED) {
	}
}

void ColorPicking::UpdateGizmo(bool picking)
{
	if (!activeCamera) return;

	float dist = activeCamera->DistTo(selectedObject);
	glm::vec3 scale(dist);
	if (GEVENT == GIZMO_EVENT::ROTATE) {
		scale.y = picking ? 2 * dist : dist / 4;
	}
	for (int i = 0; i < 3; i++) {
		gizmo[i].gizmo_object[GEVENT]->transform->SetScale(scale);
		gizmo[i].gizmo_object[GEVENT]->transform->SetWorldPosition(selectedObject->transform->GetWorldPosition());
	}
	gizmoObject->SetScale(scale);
	gizmoObject->transform->SetWorldPosition(selectedObject->transform->GetWorldPosition());
}

void ColorPicking::UpdateGizmoPosition()
{
	auto pos = selectedObject->transform->GetWorldPosition();
	for (int i = 0; i < 3; i++) {
		gizmo[i].gizmo_object[GEVENT]->transform->SetWorldPosition(pos);
	}
	gizmoObject->transform->SetWorldPosition(pos);
}

void ColorPicking::UpdateGizmoRotation()
{
	//for (int i = 0; i < 3; i++)
	//{
		//GameObject *obj = gizmo[i].gizmo_object[GEVENT];
		//glm::vec3 euler = glm::vec3(90.0f * gizmo[i].axis) * TO_RADIANS;
		//auto rotation = glm::quat(euler);
		//rotation = glm::inverse(rotation) * selectedObject->transform->GetWorldRotation();
		//obj->transform->SetWorldRotation(rotation);
	//}
	gizmoObject->transform->SetWorldRotation(selectedObject->transform->GetWorldRotation());
}

void ColorPicking::ResetGizmoRotation()
{
	for (int i = 0; i < 3; i++) {
		for (auto EVENT : events) {
			gizmo[i].gizmo_object[EVENT]->transform->SetWorldRotation(90.0f * gizmo[i].axis);
		}
	}
}

void ColorPicking::DrawSceneForPicking() const
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	cpShader->Use();

	activeCamera->BindPosition(cpShader->loc_eye_pos);
	activeCamera->BindViewMatrix(cpShader->loc_view_matrix);
	activeCamera->BindProjectionMatrix(cpShader->loc_projection_matrix);

	for (auto *obj : Manager::GetScene()->GetFrustrumObjects()) {
		if (obj->meshRenderer && obj->meshRenderer->mesh->meshType == MESH_TYPE::STATIC)
		{
			obj->RenderForPicking(cpShader);
		}
	}

	// Render Skinned meshes
	cpShaderSk->Use();
	activeCamera->BindPosition(cpShaderSk->loc_eye_pos);
	activeCamera->BindViewMatrix(cpShaderSk->loc_view_matrix);
	activeCamera->BindProjectionMatrix(cpShaderSk->loc_projection_matrix);

	for (auto *obj : Manager::GetScene()->GetFrustrumObjects()) {
		if (obj->meshRenderer && obj->meshRenderer->mesh->meshType == MESH_TYPE::SKINNED) {
			obj->RenderForPicking(cpShaderSk);
		}
	}

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	cpShader->Use();

	for (auto obj : Manager::Scene->GetSceneObjects()) {
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

	if (GEVENT == GIZMO_EVENT::ROTATE)
	{
		Manager::RenderSys->SetGlobalCulling(OpenGL::CULL::FRONT);
		GL_Utils::SetColorUnit(shader->loc_debug_color, 0.2f, 0.2f, 0.2f);
		for (int i = 0; i < 3; i++)
		{
			GameObject *obj = gizmo[i].gizmo_object[GEVENT];
			obj->Render(shader);
		}
	}

	Manager::RenderSys->SetGlobalCulling(OpenGL::CULL::BACK);
	for (int i = 0; i < 3; i++)
	{
		GameObject *obj = gizmo[i].gizmo_object[GEVENT];
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
		GameObject *obj = gizmo[i].gizmo_object[GEVENT];
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