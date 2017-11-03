#pragma once
#include <Component/Transform/Transform.h>

class DLLExport FreezeTransform
	: public Transform
{
	public:
		FreezeTransform();
		FreezeTransform(const Transform &trasform);

		// ****************************
		// Continuous transform methods

		void Move(const glm::vec3 dir, float deltaTime) {};
		void Scale(float deltaTime) {};

		virtual void RotateLocalOY(float deltaTime) {};
		virtual void RotateLocalOZ(float deltaTime) {};
		virtual void RotateWorldOX(float deltaTime) {};
		virtual void RotateWorldOY(float deltaTime) {};
		virtual void RotateWorldOZ(float deltaTime) {};
		virtual void RotateLocalOX(float deltaTime) {};

		virtual void SetLocalPosition(glm::vec3 position) {};
		virtual void SetWorldPosition(glm::vec3 position) {};
		virtual void SetWorldRotation(glm::quat rotationQ) {};

		virtual void SetRelativeRotation(glm::quat localRotationQ) {};

		// Set rotation using degree angles (0 - 360)
		void SetWorldRotation(glm::vec3 eulerAngles360) {};
		void SetWorldRotationAndScale(glm::quat rotationQ, glm::vec3 scale) {};

		void SetScale(glm::vec3 scale) {};
		void ForceUpdate() {};

		// ****************************
		// Transform properties

		void SetMoveSpeed(float speed) {};
		void SetScaleSpeed(float speed) {};
		void SetRotationSpeed(float speed) {};
};
