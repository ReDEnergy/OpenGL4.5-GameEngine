#pragma once

#include <vector>
#include <include/glm.h>

class Camera;
class Shader;
class GameObject;
class DirectionalLight;

class FrustumSplit
{
	public:
		FrustumSplit();
		~FrustumSplit();

		void Update(DirectionalLight * Ref);
		void SetCamera(const Camera* camera);

		void RenderDebug(const Shader *shader) const;
		void BindViewProjection(const Shader *shader, unsigned int index) const;
		void BindForUse(const Shader *shader) const;
		unsigned int GetNrSections() const;

	private:
		void SplitFrustrum();
		void UpdateBoundingBox(DirectionalLight * dirLight) const;

	private:
		const Camera* viewCamera;

		unsigned int nrFrustumSections;
		std::vector<float> splitDistances;
		std::vector<GameObject*> zones;

		std::vector<glm::mat4> lightProjections;
		std::vector<glm::mat4> lightViews;
};

