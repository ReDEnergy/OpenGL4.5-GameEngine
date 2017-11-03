#pragma once
#include <include/dll_export.h>
#include <include/glm.h>

class GameObject;
class Transform;
class Shader;

/**
 *	Axis Aligned Bounding Box
 *	default axis are the world axis (OX, OY, OZ)
 *	using a rotation quaternion a different axis can be provided
 */
class DLLExport AABB
{
	public:
		AABB(GameObject *obj);
		~AABB();

		static void Init();

		const Transform* GetTransform() const;
		void Render(const Shader *shader) const;
		void RenderObjectBoundingBox(const Shader * shader) const;
		void Update(const glm::quat &rotationQ);
		void ComputeLocal();
		bool Overlaps(AABB *aabb);

	private:
		void UpdateChildren(const glm::quat &rotationQ);

	private:
		Transform *transform;
		GameObject *gameObj;
		glm::vec3 center;
		glm::vec3 halfSize;
};