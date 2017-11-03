#pragma once
#include <Component/Transform/Transform.h>
#include <functional>
#include <include/glm.h>

class DLLExport LimitedTransform
	: public Transform
{
	public:
		LimitedTransform();
		LimitedTransform(const Transform &trasform);
		virtual ~LimitedTransform();

		void LimitAxisRotation(glm::vec3 axis);

		void SetWorldRotation(glm::quat rotationQ) override;
		void SetRelativeRotation(const glm::quat &rotationQ) override;

	private:
		bool applyLimits;
		glm::vec3 constraintAxis;
		std::function<glm::quat(const glm::quat &rot)> RotLimitFunc;
};
