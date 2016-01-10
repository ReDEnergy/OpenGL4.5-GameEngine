#include <pch.h>
#include "PlaneIntersection.h"

#include <include/utils.h>
#include <include/math.h>

#include <Core/Engine.h>
#include <Core/Camera/Camera.h>
#include <Core/WindowObject.h>
#include <Component/Transform/Transform.h>

#include <Manager/Manager.h>
#include <Manager/SceneManager.h>
#include <Manager/ShaderManager.h>
#include <Debugging/TextureDebugger.h>
#include <UI/ColorPicking/ColorPicking.h>

#include <GPU/Shader.h>
#include <GPU/SSBO.h>
#include <GPU/Texture.h>
#include <GPU/FrameBuffer.h>

PlaneIntersection::PlaneIntersection(const FrameBuffer *FBO)
{
	gameFBO = FBO;

	computeShader = Manager::GetShader()->GetShader("PlaneIntersection");
	if (!computeShader) {
		return;
	}

	computeShader->OnLoad([this]()
	{
		sphere_size = computeShader->GetUniformLocation("sphere_size");
		plane_origin = computeShader->GetUniformLocation("plane_origin");
		plane_direction = computeShader->GetUniformLocation("plane_direction");
	});


	sphereSize = 0;
	auto rezolution = gameFBO->GetResolution();

	visualization = new Texture();
	visualization->Create2DTexture((unsigned char*)nullptr, rezolution.x, rezolution.y, 1);

	Manager::GetTextureDebugger()->SetChannelIndex(3, 1, visualization);

	ssbo = new SSBO<glm::ivec4>(2048);

	SubscribeToEvent(EventType::FRAME_AFTER_RENDERING);
}

void PlaneIntersection::SetPlaneTransform(const Transform * const planeTransform)
{
	this->planeTransform = planeTransform;
}

void PlaneIntersection::Update()
{
	sphereSize += 0.02f;
	if (sphereSize >= 10)
		sphereSize = 1;

	int WORK_GROUP_SIZE = 32;
	computeShader->Use();

	auto rezolution = gameFBO->GetResolution();
	auto camera = Manager::GetScene()->GetActiveCamera();

	auto cameraPos = camera->transform->GetWorldPosition();
	auto planePos = cameraPos + camera->transform->GetLocalOZVector() * 3.0f;
	auto direction = camera->transform->GetLocalOZVector();
	glUniform3f(plane_direction, direction.x, direction.y, direction.z);
	glUniform3f(plane_origin, planePos.x, planePos.y, planePos.z);
	glUniform1f(sphere_size, sphereSize);
	glUniform3f(computeShader->loc_eye_pos, cameraPos.x, cameraPos.y, cameraPos.z);
	gameFBO->SendResolution(computeShader);

	ssbo->ClearBuffer();
	ssbo->BindBuffer(0);
	glBindImageTexture(0, visualization->GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8UI);
	glBindImageTexture(1, gameFBO->GetTextureID(3), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(2, Manager::GetPicker()->FBO->GetTextureID(0), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glDispatchCompute(GLuint(UPPER_BOUND(rezolution.x, WORK_GROUP_SIZE)), GLuint(UPPER_BOUND(rezolution.y, WORK_GROUP_SIZE)), 1);
	glFinish();
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	ssbo->ReadBuffer();

	auto size = ssbo->GetSize();
	auto values = ssbo->GetBuffer();
	vector<glm::vec4> positions;
	for (uint i = 0; i < size; i++) {
		if (values[i].w) {
			glm::vec4 pos = glm::vec4(values[i]) / (float(1000 * values[i].w));
			pos += glm::vec4(cameraPos, 0);
			pos.w = (float)i;
			positions.push_back(pos);
		}
	}
}

void PlaneIntersection::OnEvent(EventType eventID, void * data)
{
	if (eventID == EventType::FRAME_AFTER_RENDERING)
		Update();
}
