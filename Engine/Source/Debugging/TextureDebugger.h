#pragma once

#include <include/dll_export.h>
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
		void SetChannelIndex(unsigned int channel, unsigned int index, const Texture * const texture);
		void PushToChannel(unsigned int channel, const Texture * const texture);
		void SetChannel(unsigned int channel, const FrameBuffer* const frameBuffer);
		void SetRenderingVAO(unsigned int VAO);
		void Render();

		bool Toggle();
		bool ToggleFullScreen();
		bool IsFullScreen();
		void SelectChannel(uint channelID);
		void SelectTextureChannel(uint textureChannel);

	private:
		void OnKeyPress(int key, int mods);

	private:
		unsigned int VAO;
		bool state;
		bool fullScreen;
		uint activeChannel;
		uint activeTexture;
		GameObject *renderingQuad;
		Shader *shader;

		vector<const Texture* const> channels[TEXTURE_DEBUGGER_CHANNELS];
};