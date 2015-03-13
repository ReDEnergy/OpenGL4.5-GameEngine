#include "Terrain.h"

Terrain::Terrain() {

	position = glm::vec3(0.0f);
	rotation = glm::vec3(0.0f, 1.0f, 0.0f);
	scale = glm::vec3(1.0f);
	angle = 0;

	tess_inner_factor = 1;
	tess_outer_factor = 1;
	displacement_factor = 100;

}

Terrain::~Terrain() {

}

void Terrain::generate(int width, int height) {

	vector<glm::vec3> Positions;
	vector<glm::vec2> TexCoords;
	vector<unsigned int> Indices;


	int fwidth = width - 1;
	int fheight = height - 1;

	n_indices = 6 * fwidth * fheight;
	n_vertices = width * height;

	Positions.reserve(width * height);
	TexCoords.reserve(width * height);
	Indices.reserve(n_indices);

	// Create VAO
	glGenVertexArrays(1, &VAO);   
	glBindVertexArray(VAO);

	// Create buffers objects
	glGenBuffers(3, gl_buffers);

	float centerX = fwidth / 2.0f;
	float centerZ = fheight / 2.0f;

	for (int z = 0; z < height; z++) {
		for (int x = 0; x < width; x++) {
			Positions.push_back(glm::vec3(x - centerX, 0.0f, z - centerZ));
			TexCoords.push_back(glm::vec2((float)x / fwidth, (float)z / fheight));
		}
	}

	// Indices from Quad to Triangle
	const unsigned int q2t[] = {0, width, 1 + width, 1 + width, 1, 0};

	unsigned int face = 0;

	for (int i = 0; i < fheight; i++) {
		face = i * width;
		for (int j = 0; j < fwidth; j++) {
			Indices.push_back(face + q2t[0]);
			Indices.push_back(face + q2t[1]);
			Indices.push_back(face + q2t[2]);
			Indices.push_back(face + q2t[3]);
			Indices.push_back(face + q2t[4]);
			Indices.push_back(face + q2t[5]);
			face +=1;
		}
	}

	// Generate and populate the buffers with vertex attributes and the indices
	glBindBuffer(GL_ARRAY_BUFFER, gl_buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);    

	glBindBuffer(GL_ARRAY_BUFFER, gl_buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_buffers[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);	

}

void Terrain::render(Shader *shader) {
	computeModelMatrix();
	glUniformMatrix4fv(shader->loc_model_matrix, 1, false, glm::value_ptr(Model));
	glUniform1ui(shader->loc_displacement_factor, displacement_factor);

	glBindVertexArray(VAO);
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	glDrawElements(GL_PATCHES, n_indices, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);

}

void Terrain::computeModelMatrix() {
	Model = glm::translate(mat4_Identity, position);
	Model = glm::rotate(Model, angle, rotation);
	Model = glm::scale(Model, scale);
}