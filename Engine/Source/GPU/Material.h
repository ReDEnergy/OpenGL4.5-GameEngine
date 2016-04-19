#pragma once

#include <include/glm.h>
#include <GPU/Shader.h>
#include <GPU/Texture.h>

struct MaterialBlock
{
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 emissive;
	float shininess;
};

class Material : public MaterialBlock {
	public:
		Material();
		~Material();
		void createUBO();
		void BindUBO(GLuint location);
		void LinkShader(Shader* const S);

	public:
		Shader *shader;
		GLuint material_ubo;
		Texture* texture;
};
