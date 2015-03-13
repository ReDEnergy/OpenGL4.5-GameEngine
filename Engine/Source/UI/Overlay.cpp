//#include <pch.h>
#include "Overlay.h"

#include <Component/Text.h>
#include <Component/Transform.h>

#include <Manager/Manager.h>
#include <Manager/ShaderManager.h>

#include <GPU/Shader.h>

static glm::mat4 pmat = glm::perspective(25.0f, 1.6f, 0.1f, 50.0f);

Overlay::Overlay() {
	messageTime = 0;

	message = new Text();
	message->SetText("Great job!");
	message->transform->position = glm::vec3(-1.0f, 1.3f, 0);
	message->transform->scale *= 1.5;
	message->transform->Update();

	messages.push_back(message);

	message = new Text();
	message->SetText("Collect all light orbs!");
	message->transform->position = glm::vec3(-1.0f, 1.3f, 0);
	message->transform->scale *= 1.5;
	message->transform->Update();
	messages.push_back(message);

	SubscribeToEvent(EventType::LEVEL_FISIHED);
	SubscribeToEvent(EventType::WARNING_COLLECT_EVERYTHING);
}

Overlay::~Overlay() {
}

void Overlay::Update(float deltaTime) {
	glm::vec3 color = glm::vec3(0.967f, 0.333f, 0.486f);
	glm::vec3 selectColor = glm::vec3(0.967f, 0.873f, 0.486f);

	Shader *shader = Manager::Shader->GetShader("font");
	shader->Use();
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, false, glm::value_ptr(pmat));
	glUniform3f(shader->text_color, 0.967f, 0.333f, 0.486f);
	glDisable(GL_DEPTH_TEST);
		if (messageTime > 0) {
			messageTime -= deltaTime;
			message->Render(shader);
		}
	glEnable(GL_DEPTH_TEST);
}

void Overlay::OnEvent(EventType Event, Object *data) {
	switch (Event)
	{
		case EventType::LEVEL_FISIHED:
			messageTime = 3;
			message = messages[0];
			return;
		case EventType::WARNING_COLLECT_EVERYTHING:
			messageTime = 3;
			message = messages[1];
		default:
			break;
	}
}


