//#include <pch.h>
#include "GPU.h"

namespace UtilsGPU {

	DLLExport GPUBuffers* UploadData(const vector<glm::vec3> &positions,
					const vector<glm::vec3> &normals, 
					const vector<unsigned short>& indices)
	{
		GPUBuffers *buffers = new GPUBuffers();
		glBindVertexArray(buffers->VAO);

		// Generate and populate the buffers with vertex attributes and the indices
		glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers->VBO[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);

		assert(CheckOpenGLError() == GL_NO_ERROR);

		return buffers;
	}

	DLLExport GPUBuffers* UploadData(const vector<glm::vec3> &positions,
					const vector<glm::vec2> &text_coords, 
					const vector<unsigned short>& indices)
	{
		// Create the VAO
		GPUBuffers *buffers = new GPUBuffers();
		glBindVertexArray(buffers->VAO);

		// Generate and populate the buffers with vertex attributes and the indices
		glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(text_coords[0]) * text_coords.size(), &text_coords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers->VBO[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);

		assert(CheckOpenGLError() == GL_NO_ERROR);

		return buffers;
	}

	DLLExport GPUBuffers* UploadData(const vector<glm::vec3> &positions,
					const vector<glm::vec3> &normals,
					const vector<glm::vec2> &text_coords,
					const vector<unsigned short> &indices)
	{
		// Create the VAO
		GPUBuffers *buffers = new GPUBuffers();
		glBindVertexArray(buffers->VAO);

		// Generate and populate the buffers with vertex attributes and the indices
		glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions[0]) * positions.size(), &positions[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);    

		glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(text_coords[0]) * text_coords.size(), &text_coords[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals[0]) * normals.size(), &normals[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers->VBO[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);

		assert(CheckOpenGLError() == GL_NO_ERROR);

		return buffers;
	}

	// TODO return BUFFERS reference in order to free memory

	DLLExport void DrawLine(glm::vec3 posA, glm::vec3 posB) {
		glBegin(GL_LINES);
			glVertex3f(posA.x, posA.y, posA.z);
			glVertex3f(posB.x, posB.y, posB.z);
		glEnd();
	}

	DLLExport void DrawPolygon(vector<glm::vec3> &vertices) {
		glBegin(GL_LINES);
			// Draw polygon edges
			for (auto vertex: vertices)
				glVertex3f(vertex.x, vertex.y, vertex.z);
			// Close polygon
			glVertex3f(vertices[0].x, vertices[0].y, vertices[0].z);
		glEnd();
	}

}