#include "TextureDebugger.h"

#include <iostream>
using namespace std;

#include <include/utils.h>
#include <include/gl_utils.h>

#include <Core/Camera/Camera.h>
#include <Core/GameObject.h>
#include <Component/Transform/Transform.h>

#include <GPU/Shader.h>
#include <GPU/Texture.h>
#include <GPU/FrameBuffer.h>

#include <Manager/Manager.h>
#include <Manager/ShaderManager.h>
#include <Manager/RenderingSystem.h>
#include <Manager/ResourceManager.h>
#include <Manager/SceneManager.h>

#define MAX_NR_TEXTURES 12  

void RenderScreenQuat(GLuint VAO)
{
	glBindVertexArray(VAO);
	glDrawElementsBaseVertex(4, 6, GL_UNSIGNED_SHORT, 0, 0);
	glBindVertexArray(0);
	//CheckOpenGLError();
}

TextureDebugger::TextureDebugger()
{
	activeChannel = 0;
	activeTexture = 0;
	fullScreen = false;
}

TextureDebugger::~TextureDebugger() {
}

void TextureDebugger::Init()
{
	VAO = 0;

	shader = Manager::Shader->GetShader("debug");

	// Rendering Quad
	renderingQuad = Manager::Resource->GetGameObject("render-quad");
	renderingQuad->transform->SetScale(glm::vec3(0.5));
	renderingQuad->transform->SetWorldPosition(glm::vec3(0.5, 0.5, 0));
	renderingQuad->UseShader(shader);

	for (unsigned i = 0; i < TEXTURE_DEBUGGER_CHANNELS; i++) {
		channels[i].reserve(MAX_NR_TEXTURES);
	}
}

void TextureDebugger::SetChannelIndex(unsigned int channel, unsigned int index, const Texture* const texture)
{
	if (channel > 9) {
		cout << "[ERROR]: incorrect parameter [channel], must be between 0 and 9";
		return;
	}
	if (index > 12) {
		cout << "[ERROR]: incorrect parameter [index], must be between 0 and 11";
		return;
	}
	if (index >= channels[channel].size())
		channels[channel].resize(index + 1);
	channels[channel][index] = texture;
}

void TextureDebugger::PushToChannel(unsigned int channel, const Texture* const texture)
{
	if (channel > 9) {
		cout << "[ERROR]: incorrect parameter [channel], must be between 0 and 9";
		return;
	}
	channels[channel].push_back(texture);
}

void TextureDebugger::SetChannel(unsigned int channel, const FrameBuffer * const frameBuffer)
{
	if (channel > 9) {
		cout << "[ERROR]: incorrect parameter [channel], must be between 0 and 9";
		return;
	}
	int size = frameBuffer->GetNumberOfRenderTargets();
	channels[channel].clear();
	channels[channel].reserve(MAX_NR_TEXTURES);
	channels[channel].push_back(frameBuffer->GetDepthTexture());
	for (auto i = 0; i < size; i++)
		channels[channel].push_back(frameBuffer->GetTexture(i));
}

void TextureDebugger::SetRenderingVAO(unsigned int VAO)
{
	this->VAO = VAO;
}

void TextureDebugger::Render()
{
	if (Manager::RenderSys->Is(RenderState::DEBUG))
	{
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		shader->Use();
		glUniform1i(shader->loc_debug_id, activeTexture);

		Manager::Scene->GetActiveCamera()->BindProjectionDistances(shader);

		unsigned int size = channels[activeChannel].size();
		for (unsigned int i = 0; i < size; i++)
		{
			if (channels[activeChannel][i])
				channels[activeChannel][i]->BindToTextureUnit(GL_TEXTURE0 + i);
		}

		if (VAO) {
			glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(renderingQuad->transform->GetModel()));
			RenderScreenQuat(VAO);
		}
		else {
			renderingQuad->Render(shader);
		}

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		CheckOpenGLError();
	}
}

bool TextureDebugger::IsFullScreen()
{
	return fullScreen;
}

bool TextureDebugger::Toggle()
{
	state = Manager::RenderSys->Toggle(RenderState::DEBUG);
	return state;
}

bool TextureDebugger::ToggleFullScreen()
{
	fullScreen = !fullScreen;

	glm::vec3 scale(1.0f);
	glm::vec3 position(0);
	if (!fullScreen) {
		scale = glm::vec3(0.5);
		position = glm::vec3(0.5, 0.5, 0);
	}
	renderingQuad->transform->SetScale(scale);
	renderingQuad->transform->SetWorldPosition(position);
	return fullScreen;
}

void TextureDebugger::SelectChannel(uint channelID)
{
	if (channelID >= 0 && channelID < 11) {
		if (channels[channelID].size()) {
			activeChannel = channelID;
			for (uint i = 0; i < channels[channelID].size(); i++) {
				if (channels[channelID][i]) {
					activeTexture = i;
					break;
				}
			}
		}
	}
}

void TextureDebugger::SelectTextureChannel(uint textureChannel)
{
	if (channels[activeChannel].size() > textureChannel && channels[activeChannel][textureChannel]) {
		activeTexture = textureChannel;  // GL_TEXTURE_0 + offset;
	}
}

void TextureDebugger::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_GRAVE_ACCENT)
	{
		Toggle();
		return;
	}

	if (Manager::RenderSys->Is(RenderState::DEBUG))
	{
		if (key == GLFW_KEY_EQUAL)
		{
			if (!IsFullScreen())
				ToggleFullScreen();
			return;
		}
		if (key == GLFW_KEY_MINUS)
		{
			if (IsFullScreen())
				ToggleFullScreen();
			return;
		}

		int val = key - GLFW_KEY_0;
		if (val >= 0 && val <= 9) {
			SelectTextureChannel(val);
		}

		val = key - GLFW_KEY_F1;
		if (val >= 0 && val < 11) {
			SelectChannel(val);
		}
	}
}
