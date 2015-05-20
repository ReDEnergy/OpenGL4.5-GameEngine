#pragma once
#include <include/dll_export.h>
#include <include/glm.h>

#include <Core/Object.h>
#include <Core/GameObject.h>

enum class LightType {
	POINT_LIGHT,
	DIRECTIONAL_LIGHT,
	SPOT_LIGHT
};

class DLLExport Light: virtual public GameObject
{
	public:
		Light();
		Light(const GameObject &gameObject);
		virtual ~Light();

		void Move(const glm::vec3 dir, float delta_time);
		void RandomDiffuseColor();
		virtual void RenderDebug(const Shader *shader) const;

	protected:
		bool active;	

	public:
		GameObject *light;
		glm::vec3 diffuseColor;
		glm::vec3 bulbSize;
		LightType type;
};