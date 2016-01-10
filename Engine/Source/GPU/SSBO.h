#pragma once

#define SURFACE_PRO_TABLET

#include <include/gl.h>
#include <include/utils.h>

template <class StorageEntry>
class SSBO
{
	public:
		SSBO(unsigned int size)
		{
			this->size = size;
			auto entrySize = sizeof(StorageEntry);
			auto totalSize = size * sizeof(StorageEntry);
			data = new StorageEntry[size];

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

		void BindBuffer(GLuint index)
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

		const StorageEntry* GetBuffer()
		{
			return data;
		}

		unsigned int GetSize() const
		{
			return size;
		}

		void ClearBuffer() const
		{
			Bind();
			// Clear Buffer Object using a compute shader
			// Better should clear by setting null data
			// Reason: Intel HD4000 OpenGL glClearBufferdata (Surface PRO) will crash the program
			#ifdef SURFACE_PRO_TABLET 
			{
				Shader *S = Manager::GetShader()->GetShader("ClearBuffer");
				S->Use();
				glDispatchCompute(size, 1, 1);
				glMemoryBarrier(GL_ALL_BARRIER_BITS);
			}
			#else
			glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, NULL);
			CheckOpenGLError();
			#endif
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
		StorageEntry *data;
};