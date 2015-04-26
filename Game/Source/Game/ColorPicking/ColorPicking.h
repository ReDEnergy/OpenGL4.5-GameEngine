#pragma once

#include <Component/ObjectInput.h>

#include <include/glm.h>


class Camera;
class FrameBuffer;
class Shader;

class ColorPicking : public ObjectInput {

public:
	ColorPicking();
	~ColorPicking();

	void Init();
	void Update(const Camera* camera);
	void OnMouseBtnEvent(int mouseX, int mouseY, int button, int action, int mods);

	FrameBuffer *FBO;

private:
	Shader *cpShader;
	glm::ivec2 mousePosition;
	bool pickEvent;
};
