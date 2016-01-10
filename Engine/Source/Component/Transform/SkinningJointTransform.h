#pragma once
#include <Component/Transform/Transform.h>

using namespace std;

class DLLExport SkinningJointTransform
	: public Transform
{
	public:
		SkinningJointTransform();
		virtual ~SkinningJointTransform();
		
		glm::vec3 GetWorldPosition() const;

		// ****************************
		// Exact Transform Methods

		// * WARNING!! - Setting the Model is just a hack in order to preserve model value for certain operation
		// * where only the model matrix is given (example: Assimp library bone offsets) 
		// * Setting the model will not update internal state (position, rotation, scale)
		// * OBSERVATION: Scale and rotation can't be recomputed easy or perfect
		// * USE WITH CARE 
		void SetFakeModel(const glm::mat4& model);

	protected:
		void UpdateChildsPosition();
		void UpdateChildrenRotation(glm::quat diffQ);

	private:
		bool fakeModel;
};
