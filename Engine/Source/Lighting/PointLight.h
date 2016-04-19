#pragma once
#include <Lighting/Light.h>
#include <include/gl_defines.h>

class Camera;
class Texture;
class FrameBuffer;
class Shader;

class DLLExport PointLight : public Light
{
	public:
		PointLight();
		PointLight(const GameObject &obj);

		void InitCaster();
		void CastShadows();
		void RenderDeferred(Shader *shader);
		void BindForUse(const Shader *shader) const;
		void BindTexture(GLenum textureUnit) const;
		void SetArea(float radius);

		// Gameobject
		void SetDebugView(bool value);

	public:
		float effectRadius;

	private:
		Camera *camera;
		Texture *cubeTexture;
		FrameBuffer *FBO;
};

