#pragma once
#include <include/dll_export.h>
#include <include/glm.h>
#include <vector>

using namespace std;

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
		void Render(const Shader *shader) const;
		void Update();
		void Update(glm::quat rotationQ);
		bool Overlaps(AABB *aabb);

	public:
		Transform *transform;
		GameObject *obj;
		static GameObject *box;

	private:
		glm::vec3 center;
		glm::vec3 halfSize;
};