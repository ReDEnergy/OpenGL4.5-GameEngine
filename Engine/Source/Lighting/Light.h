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

		void Init();
		void Move(const glm::vec3 dir, float delta_time);
		void RandomDiffuseColor();
		virtual void Render(const Shader * shader) const;
		virtual void RenderForPicking(const Shader * shader) const;
		virtual void RenderDebug(const Shader *shader) const;

		// Gameobject
		void SetDebugView(bool value) = 0;

	protected:
		bool active;	

	public:
		glm::vec3 diffuseColor;
		glm::vec3 bulbSize;
		LightType type;
};