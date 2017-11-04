#pragma once
#include <Component/Transform/Transform.h>

class JointTransform
	: public Transform
{
	public:
		JointTransform();
		virtual ~JointTransform();
		
		glm::vec3 GetConstraintDirection() const;
		float GetConstraintAngle() const;

		void LimitAxisRotation(glm::vec3 axis);
		void SetConstraint(glm::vec3 direction, float coneAngle);
		void SetChildrenRotation(glm::quat rotationQ);
		void SetWorldRotation(glm::quat rotationQ);

	protected:
		void RotateChildren(glm::quat rotationQ);

	private:
		glm::vec3 constraintAxis;
		glm::vec3 direction;
		float coneAngle;
};
