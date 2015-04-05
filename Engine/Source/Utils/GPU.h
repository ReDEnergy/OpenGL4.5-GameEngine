#pragma once
#include <include/gl.h>
#include <include/glm.h>
#include <include/dll_export.h>
#include <include/utils.h>
#include <vector>

using namespace std;

namespace BUFFERTYPE {
	enum _BUFFERTYPE {
		INDEX,
		POS,
		NORMAL,
		TEXT_COORD,
		TANGENT,
		BITANGENT,
		BONE
	};
}

// Attention! - these bindings should mirror shader bindings

namespace VERTEX_ATTRIBUTE_LOC {
	enum _VERTEX_ATTRIBUTE_LOC {
		POS,
		TEX_COORD,
		NORMAL,
		BONE_ID,
		BONE_WEIGHT
	};
}

class GPUBuffers
{
	public:
		GPUBuffers(unsigned int size)
			: size(size)
		{
			VBO = new GLuint[size];
			glGenVertexArrays(1, &VAO);
			glGenBuffers(size, VBO);
		}
		~GPUBuffers() {
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(size, VBO);
			SAFE_FREE(VBO);
		}

	public:
		const unsigned short size;
		GLuint VAO;
		GLuint *VBO;
};

namespace UtilsGPU {

	GPUBuffers* UploadData(const vector<glm::vec3> &positions, 
							const vector<glm::vec3> &normals,
							const vector<unsigned short>& indices);

	GPUBuffers* UploadData(const vector<glm::vec3> &positions,
							const vector<glm::vec2> &text_coords,
							const vector<unsigned short> &indices);

	GPUBuffers* UploadData(const vector<glm::vec3> &positions,
							const vector<glm::vec3> &normals,
							const vector<glm::vec2> &text_coords,
							const vector<unsigned short> &indices);

	template<class T>
	GPUBuffers* UploadData(const vector<glm::vec3> &positions,
							const vector<glm::vec3> &normals,
							const vector<glm::vec2> &text_coords,
							const vector<T> &bones,
							const vector<unsigned short> &indices)
	{
		// Create the VAO
		GPUBuffers *buffers = new GPUBuffers(5);
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

		glBindBuffer(GL_ARRAY_BUFFER, buffers->VBO[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(bones[0]) * bones.size(), &bones[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::BONE_ID);
		glVertexAttribIPointer(VERTEX_ATTRIBUTE_LOC::BONE_ID, 4, GL_INT, sizeof(T), (const GLvoid*)0);

		glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOC::BONE_WEIGHT);
		glVertexAttribPointer(VERTEX_ATTRIBUTE_LOC::BONE_WEIGHT, 4, GL_FLOAT, GL_FALSE, sizeof(T), (const GLvoid*)16);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers->VBO[4]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

		// Make sure the VAO is not changed from the outside
		glBindVertexArray(0);

		assert(CheckOpenGLError() == GL_NO_ERROR);

		return buffers;
	}


	DLLExport void DrawLine(glm::vec3 posA, glm::vec3 posB);
	DLLExport void DrawPolygon(vector<glm::vec3> &vertices);
}
