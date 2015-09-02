#pragma once
#include <Component/Transform/Transform.h>

using namespace std;

class DLLExport JointTransform
	: public Transform
{
	public:
		JointTransform();
		virtual ~JointTransform();
		
		void UpdateWorldModel();

		void UpdateChildsPosition();
		void UpdateChildsRotation(glm::quat diffQ);

		glm::vec3 GetWorldPosition() const;

		// ****************************
		// Exact Transform Methods

		// * WARNING!! - Setting the Model is just a hack in order to preserve model value for certain operation
		// * where only the model matrix is given (example: Assimp library bone offsets) 
		// * Setting the model will not update internal state (position, rotation, scale)
		// * OBSERVATION: Scale and rotation can't be recomputed easy or perfect
		// * USE WITH CARE 
		void SetFakeModel(const glm::mat4& model);

	private:
		bool fakeModel;
};
