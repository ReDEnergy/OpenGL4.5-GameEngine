#pragma once
#include <Lighting/PointLight.h>

class LightOrb: public PointLight
{
	public:
		LightOrb();
		~LightOrb();

		void Update();
		void CollectOrb();
		void SetPosition(glm::vec3 pos);

	public:
		glm::vec3 location;

	private:
		int collect;
};
