#pragma once

#define SURFACE_PRO_TABLET

#include <include/gl.h>
#include <include/utils.h>
#include <string>

template <class StorageEntry>
class SSBO
{
	public:
		SSBO(unsigned int size)
		{
			auto size_of = sizeof(StorageEntry);
			this->size = size;
			auto entrySize = size_of;
			auto totalSize = size * size_of;
			data = new StorageEntry[size];
			memset(&clearValue, 0, size_of);

			#ifdef GLEW_ARB_shader_storage_buffer_object
			{
				glGenBuffers(1, &ssbo);
				Bind();
				glBufferData(GL_SHADER_STORAGE_BUFFER, totalSize, NULL, GL_DYNAMIC_DRAW);
				Unbind();
			}
			#endif
		}

		~SSBO()
		{
			glDeleteBuffers(1, &ssbo);
			SAFE_FREE(data);
		};

		void SetBufferData(const StorageEntry *data, GLenum usage = GL_DYNAMIC_DRAW)
		{
			Bind();
			glBufferData(GL_SHADER_STORAGE_BUFFER, this->size * sizeof(StorageEntry), data, GL_DYNAMIC_DRAW);
			Unbind();
		}

		void BindBuffer(GLuint index) const
		{
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, ssbo);
		}

		void ReadBuffer()
		{
			Bind();
			GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
			memcpy(data, p, size * sizeof(StorageEntry));
			CheckOpenGLError();
			glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
			CheckOpenGLError();
			Unbind();
		}

		const StorageEntry* GetBuffer() const
		{
			return data;
		}

		unsigned int GetSize() const
		{
			return size;
		}

		void ClearBuffer() const
		{
			// Clear Buffer Object using a compute shader
			// Better should clear by setting null data
			// Reason: Intel HD4000 OpenGL glClearBufferdata (Surface PRO) will crash the program
			#ifdef SURFACE_PRO_TABLET 
			{
				Shader *S = Manager::GetShader()->GetShader("ClearBuffer");
				S->Use();
				BindBuffer(0);
				glDispatchCompute(size, 1, 1);
				glMemoryBarrier(GL_ALL_BARRIER_BITS);
			}
			#else
			Bind();
			glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, NULL);
			Unbind();
			#endif
			CheckOpenGLError();
		}

	private:
		inline void Bind() const
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
			CheckOpenGLError();
		}

		static inline void Unbind()
		{
			glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
			CheckOpenGLError();
		}

	private:
		unsigned int ssbo;
		unsigned int size;
		StorageEntry clearValue;
		StorageEntry *data;
};