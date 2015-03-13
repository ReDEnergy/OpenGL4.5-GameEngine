#pragma once
#include <string>

#include <include/gl.h>
#include <Core/Object.h>
#include <Utils/GPU.h>

using namespace std;

class Shader;
class Transform;
class GPUBuffers;

class DLLExport Text : virtual public Object {
	public:
		Text();
		virtual ~Text();

		void SetText(const char *text);
		void InitText();
		void SetOffset(Transform offset);
		void Render(Shader *shader) const;

	public:
		Transform *offset;
		Transform *transform;
		string content;

	private:
		GPUBuffers *buffers;
		GLuint atlasTextureID;
		unsigned int nr_indices;

};

