#include "DirectOpenGL.h"

#include <include/math.h>
#include <include/gl_utils.h>

#include <Core/Camera/Camera.h>
#include <Core/GameObject.h>
#include <Component/Transform/Transform.h>
#include <Component/Mesh.h>
#include <Component/MeshRenderer.h>
#include <GPU/Shader.h>
#include <Component/Renderer.h>

#include <Manager/Manager.h>
#include <Manager/ShaderManager.h>
#include <Manager/SceneManager.h>

#include <Utils/OpenGL.h>

DirectOpenGL::DirectOpenGL()
{
}

DirectOpenGL::~DirectOpenGL() {
}

void DirectOpenGL::Init()
{
	drawShader = Manager::GetShader()->GetShader("simple");
	activeShader = drawShader;

	// Line Object
	line = new GameObject();

	auto lineMesh = new Mesh();
	lineMesh->positions.push_back(glm::vec3(0));
	lineMesh->positions.push_back(glm::vec3(0, 1, 0));

	lineMesh->normals.push_back(glm::vec3(1, 0, 0));
	lineMesh->normals.push_back(glm::vec3(1, 0, 0));

	lineMesh->indices.push_back(0);
	lineMesh->indices.push_back(1);

	lineMesh->SetGLDrawMode(GL_LINES);
	lineMesh->InitFromData();
	line->SetMesh(lineMesh);
}

void DirectOpenGL::UseShader(Shader * shader)
{
	if (shader == nullptr) {
		UseDefaultShader();
		return;
	}
	activeShader = shader;
	activeShader->Use();
}

void DirectOpenGL::UseDefaultShader()
{
	activeShader = drawShader;
	activeShader->Use();
}

void DirectOpenGL::BindActiveViewProj() const
{
	auto camera = Manager::GetScene()->GetActiveCamera();
	if (camera) camera->BindViewProj(activeShader);
}

Shader * DirectOpenGL::GetActiveShader() const
{
	return activeShader;
}

void DirectOpenGL::SetLineWidth(float width) const
{
	glLineWidth(width);
}

void DirectOpenGL::SetDrawColor(uchar red, uchar green, uchar blue) const
{
	GL_Utils::SetColor256(activeShader->loc_debug_color, red, green, blue);
}

void DirectOpenGL::SetDrawColor(uint shaderLocation, uchar red, uchar green, uchar blue) const
{
	GL_Utils::SetColor256(shaderLocation, red, green, blue);
}

void DirectOpenGL::SetStartLinePosition(const glm::vec3& position) const
{
	line->transform->SetWorldPosition(position);
}

void DirectOpenGL::DrawLine(float lenght, glm::quat rotation) const
{
	line->transform->SetWorldRotationAndScale(rotation, glm::vec3(1, lenght, 1));
	line->Render(activeShader);
}

void DirectOpenGL::DrawLine(const glm::vec3 & from, const glm::vec3 & to) const
{
	auto direction = to - from;
	auto lenght = glm::length(direction);
	auto unitVec = direction / lenght;
	auto yaw = atan2(unitVec.x, unitVec.z);
	auto pitch = acos(unitVec.y);
	auto rot = glm::quat(glm::vec3(pitch, yaw, 0));
	line->transform->SetWorldPosition(from);
	line->transform->SetWorldRotationAndScale(rot, glm::vec3(1, lenght, 1));
	line->Render(activeShader);
}

void DirectOpenGL::DrawStandardAxis(const Transform * transform, const Shader* shader, float length) const
{
	if (!shader) return;

	SetStartLinePosition(transform->GetWorldPosition());
	auto rotation = transform->GetWorldRotation();

	// OY axis
	GL_Utils::SetColorUnit(shader->loc_debug_color, 0, 1, 0);
	DrawLine(length, rotation);

	// OX axis
	GL_Utils::SetColorUnit(shader->loc_debug_color, 1, 0, 0);
	DrawLine(length, rotation * glm::quat(-0.7071f, 0, 0, 0.7071f));

	// OZ axis
	GL_Utils::SetColorUnit(shader->loc_debug_color, 0, 0, 1);
	DrawLine(length, rotation * glm::quat(0, 0, 0.7071f, 0.7071f));
}

void DirectOpenGL::DrawStandardAxis(const Transform * transform, float length) const
{
	DrawStandardAxis(transform, activeShader, length);
}
