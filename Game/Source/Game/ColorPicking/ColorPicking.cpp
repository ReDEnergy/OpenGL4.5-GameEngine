#include "ColorPicking.h"

#include <Component/Mesh.h>
#include <Component/SkinnedMesh.h>

#include <Manager/Manager.h>
#include <Manager/TextureManager.h>
#include <Manager/ShaderManager.h>
#include <Manager/SceneManager.h>

#include <Core/Engine.h>
#include <Core/Camera/Camera.h>

#include <GPU/FrameBuffer.h>
#include <GPU/Texture.h>
#include <GPU/Shader.h>

ColorPicking::ColorPicking()
	: ObjectInput(InputGroup::IG_GAMEPLAY)
{

}

ColorPicking::~ColorPicking() {
}


void ColorPicking::Init() {

	glm::ivec2 resolution = Engine::Window->resolution;
	pickEvent = false;

	FBO = new FrameBuffer();
	FBO->Generate(resolution.x, resolution.y, 1);

	cpShader = Manager::GetShader()->GetShader("simple");
}

void ColorPicking::Update(const Camera* camera){

	if (pickEvent == false)
		return;
	pickEvent = false;

	float objColor[3];

	FBO->Bind();
	cpShader->Use();

	camera->BindPosition(cpShader->loc_eye_pos);
	camera->BindViewMatrix(cpShader->loc_view_matrix);
	camera->BindProjectionMatrix(cpShader->loc_projection_matrix);

	for (auto *obj : Manager::GetScene()->activeObjects) {
		glUniform4f(cpShader->loc_debug_color, obj->colorID.r, obj->colorID.g, obj->colorID.b, 0);
		obj->Render(cpShader);
	}

	glReadPixels(mousePosition.x, mousePosition.y, 1, 1, GL_RGB, GL_FLOAT, objColor);
	cout << "Object Color:" << objColor[0] * 255 << ' ' << objColor[1] * 255 << ' ' << objColor[2] * 255 << endl;
	
	FrameBuffer::Unbind();

}

void ColorPicking::OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods)
{
	
	if (button == 0 && action == 1) {
		mousePosition.x = mouseX;
		mousePosition.y = mouseY;
		pickEvent = true;
	}

}