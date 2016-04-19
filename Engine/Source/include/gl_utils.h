#pragma once
#include <include/gl.h>
#include <include/glm.h>
#include <include/utils.h>

namespace GL_Utils
{
	inline void SetColor256(GLint location, uchar red, uchar green, uchar blue, float alpha = 1.0f) {
		glUniform4f(location, red / 255.0f, green / 255.0f, blue / 255.0f, alpha);
	}

	inline void SetColor256(GLint location, const glm::vec3& color, float alpha = 1.0f) {
		glUniform4f(location, color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, alpha);
	}

	inline void SetColor256(GLint location, const glm::vec4& color) {
		glUniform4f(location, color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a);
	}

	inline void SetColorUnit(GLint location, float red, float green, float blue, float alpha = 1.0f) {
		glUniform4f(location, red, green, blue, alpha);
	}

	inline void SetColorUnit(GLint location, const glm::vec3& color, float alpha = 1.0f) {
		glUniform4f(location, color.r, color.g, color.b, alpha);
	}

	inline void SetColorUnit(GLint location, const glm::vec4& color) {
		glUniform4f(location, color.r, color.g, color.b, color.a);
	}
}