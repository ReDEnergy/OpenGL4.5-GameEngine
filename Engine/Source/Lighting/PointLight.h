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
		void InitCaster();
		void CastShadows();
		void RenderDeferred(Shader *shader);
		void BindForUse(const Shader *shader) const;
		void BindTexture(GLenum textureUnit) const;
private:
		Camera *camera;
		Texture *cubeTexture;
		FrameBuffer *FBO;
};

