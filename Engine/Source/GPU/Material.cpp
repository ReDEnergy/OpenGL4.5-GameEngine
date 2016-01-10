//#include <pch.h>
#include "Material.h"

using namespace std;

Material::Material()
{
	texture = nullptr;
	shader = nullptr;
}

Material::~Material()
{
}

void Material::createUBO()
{
	glGenBuffers(1, &material_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, material_ubo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialBlock), static_cast<MaterialBlock*>(this), GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	if (shader->loc_material != INVALID_LOC)
		glUniformBlockBinding(shader->program, shader->loc_material, 0);
};

void Material::BindUBO(GLuint location)
{
	if (shader->loc_material != INVALID_LOC)
		glUniformBlockBinding(shader->program, shader->loc_material, location);
}

void Material::LinkShader(Shader* const S) {
	shader = S;
}
