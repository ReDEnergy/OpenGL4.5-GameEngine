#pragma once

#include <Lighting/Light.h>
#include <Core/Camera/Camera.h>

class FrameBuffer;

// TODO
// CSM should be moved outside of this class to another class Rendering Technique class
// But... There will be almost nothing left in this class
// This class doesn't actually lit anything but only computes shadows... renaming the class might be a good idea

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

		const FrameBuffer* GetTextureBuffer() const;

		// Gameobject
		void SetDebugView(bool value);

	private:
		void BindForUse(const Shader *shader) const;

	private:
		FrameBuffer *FBO;
		FrustumSplit *csmFrustum;
		const Camera* viewCamera;
};

