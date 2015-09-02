//#include <pch.h>
#include "DebugOverlayText.h"

#include <Component/Text.h>
#include <Component/Transform/Transform.h>

#include <Manager/Manager.h>
#include <Manager/ShaderManager.h>

#include <GPU/Shader.h>

static glm::mat4 pmat = glm::perspective(25.0f, 1.6f, 0.1f, 50.0f);

DebugOverlayText::DebugOverlayText() {
}

DebugOverlayText::~DebugOverlayText() {
}

void DebugOverlayText::Init()
{
}

Text* DebugOverlayText::Add(const char* textID, const char *value)
{
	Text *message = new Text();
	message->SetText(value);
	message->transform->SetWorldPosition(glm::vec3(-1.0f, 1.3f, 0));

	string ID(textID);

	messages[ID] = UIText(message, true);

	return message;
}

void DebugOverlayText::Toggle(const string messageID, bool state)
{
	if (messages.find(messageID) != messages.end()) {
		messages[messageID].isActive = state;
	}
}

void DebugOverlayText::Update()
{
	glm::vec3 color = glm::vec3(0.967f, 0.333f, 0.486f);
	glm::vec3 selectColor = glm::vec3(0.967f, 0.873f, 0.486f);

	Shader *shader = Manager::Shader->GetShader("font");
	shader->Use();
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, false, glm::value_ptr(pmat));
	glUniform3f(shader->text_color, 0.967f, 0.333f, 0.486f);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	for (auto &t : messages) {
		if (t.second.isActive) {
			t.second.text->Render(shader);
		}
	}

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

void DebugOverlayText::OnEvent(const char *eventID, Object *data)
{
}

