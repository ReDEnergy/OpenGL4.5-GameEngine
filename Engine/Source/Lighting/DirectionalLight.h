#pragma once

#include <Lighting/Light.h>
#include <Core/Camera/Camera.h>

class FrameBuffer;

class DLLExport DirectionalLight : public Light, public Camera
{
	public:
		DirectionalLight();
		~DirectionalLight();

		void Init();
		void Update();
		void SetCamera(const Camera* camera);
		void CastShadows();

		void Render(const Shader *shader) const;
		void RenderDebug(const Shader *shader) const;
		void RenderForPicking(const Shader *shader) const;

		void BakeShadows(const FrameBuffer * const sceneBuffer) const;

		// Gameobject
		void SetDebugView(bool value);

	private:
		void BindForUse(const Shader *shader) const;

	public:
		const Camera* viewCamera;
		FrameBuffer *FBO;
		float distanceTo;
		vector<glm::mat4> lightProjections;
		vector<glm::mat4> lightViews;
};

