#pragma once

#include <include/glm.h>
#include <include/dll_export.h>

#include <Core/Object.h>

class AABB;
class Mesh;
class Shader;
class Physics;
class Renderer;
class Transform;
class ObjectInput;

using namespace std;

class DLLExport GameObject: virtual public Object
{
	public:
		GameObject(const char *name = nullptr);
		GameObject(const GameObject &obj);
		virtual ~GameObject();

		void Init();
		bool ColidesWith(GameObject *object);
		float DistTo(GameObject *object);

		virtual void Render() const;
		virtual void Render(const Shader *shader) const;
		virtual void RenderInstanced(const Shader *shader, unsigned int instances) const;
		virtual void RenderDebug(const Shader *shader) const {};

		void SetupAABB();
		void SetDebugView(bool value);

		virtual void Update();
		virtual void UseShader(Shader *shader);

	public:
		AABB *aabb;
		Mesh *mesh;
		Shader *shader;
		Renderer *renderer;
		Transform *transform;
		ObjectInput *input;

		glm::vec3 colorID;

#ifdef PHYSICS_ENGINE
		Physics *physics;
#endif

	private:
		char *refID;
		bool debugView;

};

// TODO idea
// Create a temporary freeze state for changing various parameters and revert