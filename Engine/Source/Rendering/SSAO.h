#pragma once
#include <Core/Object.h>
#include <GPU/FrameBuffer.h>
#include <Core/GameObject.h>

/*
 * Class Screen Space Ambient Occlusion
 */

class Camera;

class DLLExport SSAO {
	public:
		SSAO();
		~SSAO();

		void Init(int width, int height);
		void Update(const FrameBuffer *FBO, const Camera *camera) const;
		void BindTexture(GLenum TextureUnit);

	private:
		float radius;
		int kernelSize;
		glm::vec3 kernel[64];

		GameObject *screenQuad;
		Texture *RandomNoise1;
		Texture *RandomNoise2;
		Texture *computeTexture;

		FrameBuffer ssaoFBO;
};

