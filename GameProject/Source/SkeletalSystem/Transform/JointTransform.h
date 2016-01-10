#pragma once
#include <Component/Transform/Transform.h>

using namespace std;

class JointTransform
	: public Transform
{
	public:
		JointTransform();
		virtual ~JointTransform();
		
		glm::vec3 GetConstraintDirection() const;
		float GetConstraintAngle() const;

		void SetConstraint(glm::vec3 direction, float coneAngle);
		void RotateWorldOX(float deltaTime);
		void RotateWorldOY(float deltaTime);
		void RotateWorldOZ(float deltaTime);
		void RotateLocalOX(float deltaTime);
		void RotateLocalOY(float deltaTime);
		void RotateLocalOZ(float deltaTime);

		void SetChildrenRotation(glm::quat rotationQ);

	protected:
		void RotateChildren(glm::quat rotationQ);
		void UpdateChildRotation(const glm::quat & diffQ);

	private:
		glm::vec3 direction;
		float coneAngle;
};
