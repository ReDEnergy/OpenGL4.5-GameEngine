#include "ColorPicking.h"

#include <Core/GameObject.h>

#include <Component/AABB.h>
#include <Component/Mesh.h>
#include <Component/SkinnedMesh.h>
#include <Component/Text.h>
#include <Component/Transform.h>
#include <Component/ObjectInput.h>
#include <Component/Renderer.h>

#include <Manager/Manager.h>
#include <Manager/TextureManager.h>
#include <Manager/ResourceManager.h>
#include <Manager/ShaderManager.h>
#include <Manager/SceneManager.h>

#include <Core/Engine.h>
#include <Core/Camera/Camera.h>

#include <GPU/FrameBuffer.h>
#include <GPU/Texture.h>
#include <GPU/Shader.h>


struct Gizmo {
	GameObject *gizmo_object[3];
	glm::vec3 axis, color;
}gizmo[3];


ColorPicking::ColorPicking()
	: ObjectInput(InputGroup::IG_GAMEPLAY)
{

}

ColorPicking::~ColorPicking() {

}


void ColorPicking::Init() {

	selectedObject = NULL;
	gizmo_isLocal = true;
	pickEvent = false; 
	focus_event = false;
	
	focus_minSpeed = 1.0f;
	focus_maxSpeed = 5.0f;
	focus_accel = 0.25f;

	gizmo_moveCameraAlong = false;
	gizmo_action = Move;


	glm::ivec2 resolution = Engine::Window->resolution;
	
	//generate buffer for color picking
	FBO = new FrameBuffer();
	FBO->Generate(resolution.x, resolution.y, 1);

	//buffer for gizmo-only
	FBO_Gizmo = new FrameBuffer();
	FBO_Gizmo->Generate(resolution.x, resolution.y, 1);

	//get shaders
	cpShader = Manager::GetShader()->GetShader("simple");
	gizmoShader = Manager::GetShader()->GetShader("simple");

	//gizmo struct init
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
		gizmo[i].gizmo_object[Move] = Manager::GetResource()->GetGameObject("gizmo_move");
		gizmo[i].gizmo_object[Rotate] = Manager::GetResource()->GetGameObject("gizmo_rotate");
		gizmo[i].gizmo_object[Scale] = Manager::GetResource()->GetGameObject("gizmo_scale");
	}
}

void ColorPicking::Update(const Camera* activeCamera){

	camera = activeCamera;
	//draw gizmo every frame
	ColorPicking::DrawGizmo();

	if (focus_event)
		ColorPicking::FocusCamera();

	if (pickEvent == false)
		return;
	pickEvent = false;

	FBO->Bind();
	cpShader->Use();

	camera->BindPosition(cpShader->loc_eye_pos);
	camera->BindViewMatrix(cpShader->loc_view_matrix);
	camera->BindProjectionMatrix(cpShader->loc_projection_matrix);

	for (auto *obj : Manager::GetScene()->activeObjects) {
		glUniform4f(cpShader->loc_debug_color, obj->colorID.r, obj->colorID.g, obj->colorID.b, 0);
		obj->Render(cpShader);
	}

	//draw gizmo objects for color picking
	glDisable(GL_DEPTH_TEST);
	for (int i = 0; i <= 2; i++) {
		GameObject *crt_gizmo_obj = gizmo[i].gizmo_object[gizmo_action];
		
		crt_gizmo_obj->transform->SetScale((glm::vec3) glm::distance(camera->transform->position, gizmoPosition));
		crt_gizmo_obj->transform->SetPosition(gizmoPosition);
	
		if (gizmo_isLocal){
			//crt_gizmo_obj->transform->SetRotation(gizmoLocalRotation + 90.0f * gizmo[i].axis);
			crt_gizmo_obj->transform->SetRotation(90.0f * gizmo[i].axis);
		}
		else
			crt_gizmo_obj->transform->SetRotation(90.0f * gizmo[i].axis);

		glUniform4f(cpShader->loc_debug_color, crt_gizmo_obj->colorID.r, crt_gizmo_obj->colorID.g, crt_gizmo_obj->colorID.b, 0);
		crt_gizmo_obj->Render(cpShader);
	}
	glEnable(GL_DEPTH_TEST);

	//read pixel from colorpicking frame buffer
	float objColor[3];
	glReadPixels(mousePosition.x, Engine::Window->resolution.y - mousePosition.y, 1, 1, GL_RGB, GL_FLOAT, objColor);
	glm::vec3 pickedColor = glm::vec3(objColor[0], objColor[1], objColor[2]);

	//check if gizmo was clicked
	for (int i = 0; i <= 2; i++) {
		if (pickedColor == gizmo[i].gizmo_object[gizmo_action]->colorID){
			gizmoEvent = true;
			//the color matches the axis
			currentAxis = gizmo[i].color;
		}
	}
	if (!gizmoEvent){
		selectedObject = NULL;
		//check for objects click
		for (auto *obj : Manager::GetScene()->activeObjects) {
			if (obj->colorID == pickedColor) {
				selectedObject = obj;
				break;
			}
		}
	}

	FrameBuffer::Unbind();

}

void ColorPicking::OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods) {
	//on left click press
	if (button == 0 && action == 1) {
		mousePosition.x = mouseX;
		mousePosition.y = mouseY;
		pickEvent = true;
	}
	//on left click release
	if (button == 0 && action == 0){
		currentAxis = glm::vec3(0, 0, 0);
		gizmoEvent = false;
	}
}


void ColorPicking::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) {

	if (selectedObject == NULL)
		return;

	if (gizmoEvent){
		if (gizmo_action == Move) {
			float dist = glm::distance(camera->transform->position, selectedObject->transform->position);
			glm::vec3 delta((deltaX - deltaY) * dist / 500);

			selectedObject->transform->SetPosition(selectedObject->transform->position + currentAxis * delta);

			if (gizmo_moveCameraAlong == true) {
				camera->transform->SetPosition(camera->transform->position + currentAxis * delta);
			}
		}
		else if (gizmo_action == Rotate) {
			glm::vec3 rotation_vector = currentAxis * glm::vec3(deltaX - deltaY);
			selectedObject->transform->rotateSpeed = 0.03f;
			selectedObject->transform->RotateRoll(rotation_vector.x);
			selectedObject->transform->RotateYaw(rotation_vector.y);
			selectedObject->transform->RotatePitch(rotation_vector.z);
		}
		else if (gizmo_action == Scale) {
			selectedObject->transform->SetScale(selectedObject->transform->scale + currentAxis *
				glm::vec3(deltaX - deltaY) / glm::vec3(200.0f)
			);
		}
	}
}

void ColorPicking::OnKeyPress(int key, int mod)
{
	if (mod == GLFW_MOD_CONTROL){
		if (key == GLFW_KEY_W)
			gizmo_action = Move;
		else if (key == GLFW_KEY_E)
			gizmo_action = Rotate;
		else if (key == GLFW_KEY_R)
			gizmo_action = Scale;
		else if (key == GLFW_KEY_F) {
			focus_currentSpeed = focus_minSpeed;
			focus_event = true;
		}
	}
	if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_LEFT_CONTROL)
		gizmo_moveCameraAlong = true;
}

void ColorPicking::OnKeyRelease(int key, int mod)
{
	if (key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_LEFT_CONTROL)
		gizmo_moveCameraAlong = false;
}

void ColorPicking::FocusCamera() {

	if (selectedObject == NULL || focus_event == false)
		return;
		
	glm::vec3 AABB_Scale = selectedObject->aabb->transform->scale;
	float focus_dist = 3 * max( AABB_Scale.x, max (AABB_Scale.y, AABB_Scale.z) );
	glm::vec3 focus_position = selectedObject->transform->position - glm::normalize(camera->forward) * focus_dist;

	glm::vec3 cameraPosition = camera->transform->position;
	if (cameraPosition == focus_position)
		focus_event = false;
	else{
		focus_currentSpeed = min(focus_maxSpeed, focus_currentSpeed + focus_accel);
		camera->transform->SetPosition(cameraPosition + glm::normalize(focus_position - cameraPosition) * min(focus_currentSpeed, glm::distance(focus_position, cameraPosition)));
	}
		
}

void ColorPicking::DrawGizmo() {

	FBO_Gizmo->Bind();
	
	if (selectedObject != NULL){

		gizmoShader->Use();

		gizmoPosition = selectedObject->transform->position;
		gizmoLocalRotation = selectedObject->transform->eulerAngles;

		camera->BindPosition(gizmoShader->loc_eye_pos);
		camera->BindViewMatrix(gizmoShader->loc_view_matrix);
		camera->BindProjectionMatrix(gizmoShader->loc_projection_matrix);


		for (int i = 0; i <= 2; i++){
			GameObject *crt_gizmo_obj = gizmo[i].gizmo_object[gizmo_action];

			crt_gizmo_obj->transform->SetScale(glm::distance(camera->transform->position, gizmoPosition) + glm::vec3(i / 5.0f));
			crt_gizmo_obj->transform->SetPosition(gizmoPosition);

			if (gizmo_isLocal)
			{
				//crt_gizmo_obj->transform->SetRotation(gizmoLocalRotation + 90.0f * gizmo[i].axis);
				crt_gizmo_obj->transform->SetRotation(90.0f * gizmo[i].axis);
			}
			else
				crt_gizmo_obj->transform->SetRotation(90.0f * gizmo[i].axis);
			if (currentAxis == gizmo[i].color)
				glUniform4f(gizmoShader->loc_debug_color, 1, 1, 0, 1.0f);
			else
				glUniform4f(gizmoShader->loc_debug_color, gizmo[i].color.x, gizmo[i].color.y, gizmo[i].color.z, 1.0f);
			crt_gizmo_obj->Render(gizmoShader);
		}
	}

	FrameBuffer::Unbind();
}