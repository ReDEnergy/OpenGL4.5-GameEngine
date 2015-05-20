#pragma once
#include <Lighting/Light.h>
#include <include/gl.h>

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

	public:
		float effectRadius;

	private:
		Camera *camera;
		Texture *cubeTexture;
		FrameBuffer *FBO;
};

