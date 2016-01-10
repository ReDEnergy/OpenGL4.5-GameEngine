#pragma once

#include <Lighting/Light.h>
#include <Core/Camera/Camera.h>

class FrameBuffer;

class DLLExport SpotLight : public Light, public Camera
{
	public:
		SpotLight();
		~SpotLight();

		void Init();
		void Update();
		void CastShadows();
		void Render(const Shader *shader) const;
		void RenderDebug(const Shader *shader) const;
		void RenderForPicking(const Shader *shader) const;

		void SplitFrustum(unsigned int splits);

		void BakeShadows(const FrameBuffer * const sceneBuffer) const;

		// Gameobject
		void SetDebugView(bool value);

	private:
		void BindForUse(const Shader *shader) const;

	public:
		FrameBuffer *FBO;
		vector<glm::mat4> lightProjections;
		vector<glm::mat4> lightViews;
};

