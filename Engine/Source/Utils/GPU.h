#pragma once
#include <include/gl.h>
#include <include/glm.h>
#include <include/dll_export.h>
#include <vector>

using namespace std;

class DLLExport GPUBuffers
{
	public:
		GPUBuffers() {
			glGenVertexArrays(1, &VAO);
			glGenBuffers(4, VBO);
		}
		~GPUBuffers() {
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(4, VBO);
		}

	public:
		GLuint VAO;
		GLuint VBO[4];
};

namespace UtilsGPU {

	DLLExport GPUBuffers* UploadData(const vector<glm::vec3> &positions, 
					const vector<glm::vec3> &normals,
					const vector<unsigned short>& indices);

	DLLExport GPUBuffers* UploadData(const vector<glm::vec3> &positions,
					const vector<glm::vec2> &text_coords,
					const vector<unsigned short> &indices);

	DLLExport GPUBuffers* UploadData(const vector<glm::vec3> &positions,
					const vector<glm::vec3> &normals,
					const vector<glm::vec2> &text_coords,
					const vector<unsigned short> &indices);

	DLLExport void DrawLine(glm::vec3 posA, glm::vec3 posB);
	DLLExport void DrawPolygon(vector<glm::vec3> &vertices);
}
