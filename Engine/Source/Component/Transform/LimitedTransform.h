#pragma once
#include <Component/Transform/Transform.h>

using namespace std;

class DLLExport LimitedTransform
	: public Transform
{
	public:
		LimitedTransform();
		LimitedTransform(const Transform &trasform);
		virtual ~LimitedTransform();

		void SetWorldRotation(glm::quat rotationQ);

	private:
		glm::vec3 limitMax;
		glm::vec3 limitMin;

};
