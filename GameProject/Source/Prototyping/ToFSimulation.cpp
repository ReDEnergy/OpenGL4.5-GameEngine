#include "pch.h"
#include "ToFSimulation.h"

ToFSimulation::ToFSimulation()
{

	computeTexture = new Texture();

	SubscribeToEvent(EventType::FRAME_UPDATE);

	Manager::GetTextureDebugger()->SetChannelIndex(5, 1, computeTexture);

}

void ToFSimulation::Init(FrameBuffer &frameBuffer)
{
	FBO = &frameBuffer;

	auto res = FBO->GetResolution();
	computeTexture->Create2DTextureFloat(nullptr, res.x, res.y, 1, 32);
}

void ToFSimulation::Update()
{

	Shader *S = Manager::GetShader()->GetShader("ToFSimulation");
	if (S)
	{
		S->Use();

		int WORK_GROUP_SIZE = 32;
		unsigned int width, height;
		computeTexture->GetSize(width, height);

		FBO->GetDepthTexture()->BindToTextureUnit(GL_TEXTURE0);
		glBindImageTexture(0, FBO->GetTextureID(3), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
		glBindImageTexture(1, computeTexture->GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

		glDispatchCompute(GLuint(UPPER_BOUND(width, WORK_GROUP_SIZE)), GLuint(UPPER_BOUND(height, WORK_GROUP_SIZE)), 1);
		glMemoryBarrier(GL_ALL_BARRIER_BITS);
	}
}

void ToFSimulation::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_T)
	{
	}
}

void ToFSimulation::OnEvent(EventType Event, void * data)
{
	if (Event == EventType::FRAME_UPDATE) {
		Update();
	}
}
