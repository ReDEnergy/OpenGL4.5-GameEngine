#pragma once
#include <vector>
#include <chrono>

#include <include/gl.h>
#include <include/glm.h>
#include <include/dll_export.h>

#include <Core/Camera/Camera.h>
#include <Component/Transform/Transform.h>

#include <GPU/Shader.h>
#include <GPU/Texture.h>
#include <GPU/SSBO.h>

#include <Manager/Manager.h>
#include <Manager/ShaderManager.h>
#include <Manager/TextureManager.h>

template <class T>
class ParticleEffect
{
	public:
		ParticleEffect();
		virtual ~ParticleEffect();

		virtual void Generate(unsigned int particleCount);
		virtual void FillRandomData(std::function<T(void)> generator);
		virtual void Render(Camera *camera, Shader *shader, unsigned int nrParticles = -1);

		virtual SSBO<T>* GetParticleBuffer() const
		{
			return particles;
		}

		virtual unsigned int GetSize() const
		{
			return particleCount;
		}

	public:
		Transform * source;

	protected:
		unsigned int particleCount;
		GLuint VAO;
		GLuint VBO;
		SSBO<T> *particles;
};


template <class T>
ParticleEffect<T>::ParticleEffect()
{
	source = new Transform();
	particles = nullptr;
}

template <class T>
ParticleEffect<T>::~ParticleEffect()
{
	SAFE_FREE(source);
	SAFE_FREE(particles);
}

template <class T>
void ParticleEffect<T>::Render(Camera *camera, Shader *shader, unsigned int nrParticles)
{
	// Bind MVP
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(source->GetModel()));
	camera->BindViewMatrix(shader->loc_view_matrix);
	camera->BindProjectionMatrix(shader->loc_projection_matrix);
	camera->BindPosition(shader->loc_eye_pos);

	// Bind Particle Storage
	particles->BindBuffer(0);

	// Render Particles
	glBindVertexArray(VAO);
	glDrawElements(GL_POINTS, MIN(particleCount, nrParticles), GL_UNSIGNED_INT, 0);
}

template <class T>
void ParticleEffect<T>::Generate(unsigned int particleCount)
{
	this->particleCount = particleCount;

	SAFE_FREE(particles);
	particles = new SSBO<T>(particleCount);

	unsigned int *indices = new unsigned int[particleCount];
	unsigned int *p = indices;
	for (unsigned int i = 0; i < particleCount; i++)
	{
		*p = i;
		p++;
	}

	GLuint IBO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, particleCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	delete[] indices;
}

template <class T>
void ParticleEffect<T>::FillRandomData(std::function<T(void)> generator)
{
	particles->ReadBuffer();
	auto data = const_cast<T*>(particles->GetBuffer());
	for (unsigned int i = 0; i < particleCount; i++) {
		data[i] = generator();
	}
	particles->SetBufferData(data);
}