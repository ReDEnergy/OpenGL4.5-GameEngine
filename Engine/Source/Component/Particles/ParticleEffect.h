#pragma once
#include <vector>

#include <include/gl.h>
#include <include/glm.h>
#include <include/dll_export.h>

#include <Core/Camera/Camera.h>
#include <Component/Transform/Transform.h>

#include <GPU/Shader.h>
#include <GPU/Texture.h>

#include <Manager/Manager.h>
#include <Manager/ShaderManager.h>
#include <Manager/TextureManager.h>

class Particle
{
	public:
		Particle() {};
		Particle(const glm::vec4 &pos, const glm::vec4 &spd)
			: position(pos)
			, speed(spd) {}

		virtual ~Particle() {}

	public:
		glm::vec4 position;
		glm::vec4 speed;
};

template <class T>
class ParticleEffect
{
	public:
		ParticleEffect();
		virtual ~ParticleEffect();

		virtual void Generate(unsigned int particleCount);
		virtual void Render(Camera *camera, Shader *shader);
		virtual void Update();

	public:
		Texture *billboard;
		Transform *source;

	protected:
		unsigned int particleCount;
		GLuint VAO;
		GLuint VBO;
		std::vector<T> particles;
};


template <class T>
ParticleEffect<T>::ParticleEffect()
{
	source = new Transform();
	billboard = new Texture();
	billboard = Manager::GetTexture()->GetTexture("explosion.bmp");
}

template <class T>
ParticleEffect<T>::~ParticleEffect() {

}

template <class T>
void ParticleEffect<T>::Update() {

}

template <class T>
void ParticleEffect<T>::Render(Camera *camera, Shader *shader)
{
	// Bind MVP
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(source->GetModel()));
	camera->BindViewMatrix(shader->loc_view_matrix);
	camera->BindProjectionMatrix(shader->loc_projection_matrix);
	camera->BindPosition(shader->loc_eye_pos);

	// Bind Billboard Texture
	billboard->BindToTextureUnit(GL_TEXTURE0);

	glBindVertexArray(VAO);
	glDrawElements(GL_POINTS, particleCount, GL_UNSIGNED_INT, 0);
}

template <class T>
void ParticleEffect<T>::Generate(unsigned int particleCount)
{
	this->particleCount = particleCount;

	for (unsigned int i = 0; i < particleCount; i++){
		float pos_x = (rand() % 200 - 100) / 10.0f;
		float pos_y = (rand() % 200 - 100) / 10.0f;
		float pos_z = (rand() % 200 - 100) / 10.0f;
		glm::vec4 pos = glm::vec4(pos_x, pos_y, pos_z, 1);

		float spd_x = (rand() % 200 - 100) / 100.0f;
		float spd_z = (rand() % 200 - 100) / 100.0f;
		glm::vec4 spd = glm::vec4(spd_x, 5, spd_z, 0);
		particles.push_back(T(pos, spd));
	}

	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < particleCount; i++)
		indices.push_back(i);

	GLuint IBO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, particles.size()*sizeof(T), &particles[0], GL_STATIC_DRAW);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// position info
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(T), (void*)0);

	// speed info
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(T), (void*)(sizeof(float) * 4));
}