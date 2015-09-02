//#include <pch.h>
#include "GPU.h"

namespace UtilsGPU {

	GPUBuffers* UploadData(const vector<glm::vec3> &positions,
					const vector<glm::vec3> &normals, 
					const vector<unsigned short>& indices)
	{
		GPUBuffers *buffers = new GPUBuffers(3);
		glBindVertexArray(buffers->VAO);

		// Generate and populate the buffers with vertex attributes and the indices
		glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::POS);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::POS, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::TEX_COORD);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::TEX_COORD, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers->VBO[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);

		assert(CheckOpenGLError() == GL_NO_ERROR);

		return buffers;
	}

	GPUBuffers* UploadData(const vector<glm::vec3> &positions,
					const vector<glm::vec2> &text_coords, 
					const vector<unsigned short>& indices)
	{
		// Create the VAO
		GPUBuffers *buffers = new GPUBuffers(3);
		glBindVertexArray(buffers->VAO);

		// Generate and populate the buffers with vertex attributes and the indices
		glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::POS);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::POS, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(text_coords[0]) * text_coords.size(), &text_coords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::TEX_COORD);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers->VBO[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);

		assert(CheckOpenGLError() == GL_NO_ERROR);

		return buffers;
	}

	GPUBuffers* UploadData(const vector<glm::vec3> &positions,
					const vector<glm::vec3> &normals,
					const vector<glm::vec2> &text_coords,
					const vector<unsigned short> &indices)
	{
		// Create the VAO
		GPUBuffers *buffers = new GPUBuffers(4);
		glBindVertexArray(buffers->VAO);

		// Generate and populate the buffers with vertex attributes and the indices
		glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::POS);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::POS, 3, GL_FLOAT, GL_FALSE, 0, 0);    

		glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(text_coords[0]) * text_coords.size(), &text_coords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::TEX_COORD);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::TEX_COORD, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::NORMAL);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::NORMAL, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers->VBO[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);

		assert(CheckOpenGLError() == GL_NO_ERROR);

		return buffers;
	}
}

GLuint GPUBuffers::CreateNewVAO()
{
	GLuint newVAO;
	glGenVertexArrays(1, &newVAO);
	glBindVertexArray(newVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::POS);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::TEX_COORD);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[2]);

	return GLuint();
}
