#pragma once

#include <vector>

#include <include/dll_export.h>
#include <include/glm.h>
#include <include/utils.h>

#include <Component/ObjectInput.h>

class Texture;
class Transform;
class Shader;
class GameObject;
class FrameBuffer;

#define TEXTURE_DEBUGGER_CHANNELS 12 

class DLLExport TextureDebugger
	: public ObjectInput
{
	protected:
		TextureDebugger();
		~TextureDebugger();

	public:
		void Init();
		void SetChannelIndex(unsigned int channel, unsigned int index, const Texture* texture);
		void PushToChannel(unsigned int channel, const Texture* texture);
		void SetChannel(unsigned int channel, const FrameBuffer* const frameBuffer);
		unsigned int GetEmptyChannel() const;
		void SetRenderingVAO(unsigned int VAO);
		void Render();

		bool Toggle();
		bool ToggleFullScreen();
		bool IsFullScreen();
		void SelectActiveChannel(uint channelID);
		void SelectActiveTexture(uint textureIndex);
		void SetViewMode(glm::ivec4 mask);

		glm::ivec4 GetMask() const;

	private:
		void OnKeyPress(int key, int mods);

	private:
		unsigned int VAO;
		bool state;
		bool fullScreen;
		uint activeChannel;
		uint activeTexture;
		glm::ivec4 channelMask;
		GameObject *renderingQuad;

		Shader *shader;
		int loc_channel_mask;

		std::vector<const Texture*> channels[TEXTURE_DEBUGGER_CHANNELS];
};