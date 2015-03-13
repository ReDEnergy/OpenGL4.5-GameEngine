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
		void CastShadows(const Camera *camera);
		void RenderDebug(const Shader *shader) const;
		void BindForUse(const Shader *shader, Camera *camera) const;

	public:
		FrameBuffer *FBO;
		float distanceTo;
		vector<glm::mat4> lightProjections;
		vector<glm::mat4> lightViews;
};

