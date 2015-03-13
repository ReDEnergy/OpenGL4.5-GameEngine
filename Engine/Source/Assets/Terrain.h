#pragma once

#include <vector>
#include "glm.h"
#include "Globals.h"
#include "Shader.h"

using namespace std;

class Terrain
{
public:
	Terrain();
	~Terrain();

	void generate(int width, int length);
	void render(Shader *shader);
	void computeModelMatrix();

public:
	glm::mat4 Model;
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
	float angle;

	GLuint tess_inner_factor;
	GLuint tess_outer_factor;
	GLuint displacement_factor;

private:

	GLuint VAO;
	GLuint n_indices;
	GLuint n_vertices;
	GLuint gl_buffers[3];

};