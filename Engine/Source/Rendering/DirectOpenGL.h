#pragma once

#include <include/dll_export.h>
#include <include/glm.h>
#include <include/utils.h>

class Shader;
class GameObject;
class Transform;

class DLLExport DirectOpenGL
{
	protected:
		DirectOpenGL();
		~DirectOpenGL();

	public:
		void Init();

		void Use(Shader *shader = nullptr);
		Shader * GetActiveShader() const;
		void SetLineWidth(float width) const;
		void SetDrawColor(uchar red, uchar green, uchar blue) const;
		void SetDrawColor(uint shaderLocation, uchar red, uchar green, uchar blue) const;
		void SetStartLinePosition(const glm::vec3 & position) const;
		void DrawLine(float lenght, glm::quat rotation) const;
		void DrawLine(const glm::vec3& from, const glm::vec3& to) const;
		void DrawStandardAxis(const Transform * transform, const Shader * shader) const;
		void DrawStandardAxis(const Transform * transform) const;

	private:
		Shader *drawShader;
		Shader *activeShader;
		GameObject *line;
};

