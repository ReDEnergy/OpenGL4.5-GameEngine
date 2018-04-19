#include "GPU.h"

using namespace std;

namespace UtilsGPU {

	GPUBuffers UploadData(const vector<glm::vec3> &positions,
					const vector<glm::vec3> &normals, 
					const vector<unsigned int>& indices)
	{
		GPUBuffers buffers;
		buffers.Generate(3);
		glBindVertexArray(buffers.VAO);

		// Generate and populate the buffers with vertex attributes and the indices
		glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::POS);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::POS, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::TEX_COORD);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::TEX_COORD, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.VBO[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);

		CheckOpenGLError();

		return buffers;
	}

	GPUBuffers UploadData(const vector<glm::vec3> &positions,
					const vector<glm::vec2> &text_coords, 
					const vector<unsigned int>& indices)
	{
		// Create the VAO
		GPUBuffers buffers;
		buffers.Generate(3);
		glBindVertexArray(buffers.VAO);

		// Generate and populate the buffers with vertex attributes and the indices
		glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::POS);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::POS, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(text_coords[0]) * text_coords.size(), &text_coords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::TEX_COORD);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.VBO[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);

		// TODO: uncomment
		// CheckOpenGLError();

		return buffers;
	}

	GPUBuffers UploadData(const vector<glm::vec3> &positions,
					const vector<glm::vec3> &normals,
					const vector<glm::vec2> &text_coords,
					const vector<unsigned int> &indices)
	{
		// Create the VAO
		GPUBuffers buffers;
		buffers.Generate(4);
		glBindVertexArray(buffers.VAO);

		// Generate and populate the buffers with vertex attributes and the indices
		glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::POS);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::POS, 3, GL_FLOAT, GL_FALSE, 0, 0);    

		glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(text_coords[0]) * text_coords.size(), &text_coords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::TEX_COORD);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::NORMAL);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.VBO[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);
		CheckOpenGLError();

		return buffers;
	}
}

GPUBuffers::GPUBuffers()
{
	size = 0;
	VAO = -1;
	memset(VBO, 0, 6 * sizeof(int));
}

GPUBuffers::~GPUBuffers()
{
	// DO NOT REALEASE VBO HERE
}

void GPUBuffers::Generate(unsigned int size)
{
	this->size = size;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(size, VBO);
}

GLuint GPUBuffers::CreateNewVAO()
{
	GLuint newVAO;
	glGenVertexArrays(1, &newVAO);
	glBindVertexArray(newVAO);

	int k = 0;

	if (VBO[0]) {
		k++;
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::POS);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::POS, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if (VBO[1]) {
		k++;
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::TEX_COORD);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if (VBO[2]) {
		k++;
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::NORMAL);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	//if (VBO[3]) {
	//	k++;
	//	glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::BONE_ID);
	//	glVertexAttribIPointer(VERTEX_ATTRIBUTE_LOC::BONE_ID, 4, GL_INT, sizeof(T), (const GLvoid*)0);

	//	glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::BONE_WEIGHT);
	//	glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::BONE_WEIGHT, 4, GL_FLOAT, GL_FALSE, sizeof(T), (const GLvoid*)16);
	//}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[k]);

	// Make sure the VAO is not changed from the outside
	glBindVertexArray(0);
	CheckOpenGLError();

	return newVAO;
}

void GPUBuffers::ReleaseMemory()
{
	if (size) {
		size = 0;
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(size, VBO);
	}
}

GPUBuffers & GPUBuffers::operator=(const GPUBuffers & obj)
{
	size = obj.size;
	VAO = obj.VAO;
	memcpy(VBO, obj.VBO, 6 * sizeof(GLuint));
	return *this;
}
