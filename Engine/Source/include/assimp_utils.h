#pragma once
#include <include/glm.h>
#include <include/assimp.h>

namespace assimp {

	using namespace std;

	inline void CopyColor(const aiColor4D &color, glm::vec4 dest)
	{
		dest.r = color.r;
		dest.g = color.g;
		dest.b = color.b;
		dest.a = color.a;
	}

	inline void CopyMatix(const aiMatrix4x4 &mat, glm::mat4 &dest)
	{
		dest[0][0] = mat.a1; dest[0][1] = mat.b1; dest[0][2] = mat.c1; dest[0][3] = mat.d1;
		dest[1][0] = mat.a2; dest[1][1] = mat.b2; dest[1][2] = mat.c2; dest[1][3] = mat.d2;
		dest[2][0] = mat.a3; dest[2][1] = mat.b3; dest[2][2] = mat.c3; dest[2][3] = mat.d3;
		dest[3][0] = mat.a4; dest[3][1] = mat.b4; dest[3][2] = mat.c4; dest[3][3] = mat.d4;
	}

	inline aiVector3D lerp(const aiVector3D& start, const aiVector3D& end, float factor)
	{
		#ifdef _DEBUG
			assert(factor >= 0.0f && factor <= 1.0f);
		#endif
		aiVector3D delta = end - start;
		return (start + factor * delta);
	}

	inline uint GetNextAnimationKey(uint currentKey, uint maxKeys)
	{
		return (currentKey + 1) % maxKeys;
	}

	// Seems that assimp sets the minimum number of keys = 2
	// Safe mode - we return the maximum number of keys - 1
	inline uint FindPositionKeyID(float animationTime, const aiNodeAnim* pNodeAnim)
	{
		for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
			if (animationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
				return i;
			}
		}
		return pNodeAnim->mNumPositionKeys - 1;
	}

	inline uint FindRotationKeyID(float animationTime, const aiNodeAnim* pNodeAnim)
	{
		for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
			if (animationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
				return i;
			}
		}

		return pNodeAnim->mNumRotationKeys - 1;
	}

	inline uint FindScalingKeyID(float animationTime, const aiNodeAnim* pNodeAnim)
	{
		for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
			if (animationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
				return i;
			}
		}

		return pNodeAnim->mNumScalingKeys - 1;
	}

	inline const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
	{
		for (uint i = 0; i < pAnimation->mNumChannels; i++) {
			const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

			if (string(pNodeAnim->mNodeName.data) == NodeName) {
				return pNodeAnim;
			}
		}
		return NULL;
	}

	inline glm::vec3 CalcInterpolatedPosition(const aiNodeAnim* pNodeAnim, float animationTime, double animationDuration)
	{
		if (pNodeAnim->mNumPositionKeys == 1) {
			auto &value = pNodeAnim->mPositionKeys[0].mValue;
			return glm::vec3(value.x, value.y, value.z);
		}

		aiVectorKey *positionKeys = pNodeAnim->mPositionKeys;

		uint keyID = FindPositionKeyID(animationTime, pNodeAnim);
		uint nextKeyID = GetNextAnimationKey(keyID, pNodeAnim->mNumPositionKeys);

		float keyDeltaTime = 0;
		if (nextKeyID == 0) {
			keyDeltaTime = float(animationDuration - positionKeys[keyID].mTime + positionKeys[0].mTime);
		}
		else {
			keyDeltaTime = float(positionKeys[nextKeyID].mTime - positionKeys[keyID].mTime);
		}

		float factor = (animationTime - (float)positionKeys[keyID].mTime) / keyDeltaTime;

		auto value = lerp(positionKeys[keyID].mValue, positionKeys[nextKeyID].mValue, factor);
		return glm::vec3(value.x, value.y, value.z);
	}

	inline glm::quat CalcInterpolatedRotation(const aiNodeAnim* pNodeAnim, float animationTime, double animationDuration)
	{
		// we need at least two values to interpolate...
		if (pNodeAnim->mNumRotationKeys == 1) {
			auto &value = pNodeAnim->mRotationKeys[0].mValue;
			return glm::quat(value.w, value.x, value.y, value.z);
		}

		aiQuatKey *rotationKeys = pNodeAnim->mRotationKeys;

		uint rotKeyID = FindRotationKeyID(animationTime, pNodeAnim);
		uint nextRotKeyID = GetNextAnimationKey(rotKeyID, pNodeAnim->mNumRotationKeys);

		float keyDeltaTime = 0;
		if (nextRotKeyID == 0) {
			keyDeltaTime = float(animationDuration - rotationKeys[rotKeyID].mTime + rotationKeys[0].mTime);
		}
		else {
			keyDeltaTime = float(rotationKeys[nextRotKeyID].mTime - rotationKeys[rotKeyID].mTime);
		}

		float factor = (animationTime - (float)rotationKeys[rotKeyID].mTime) / keyDeltaTime;
		assert(factor >= 0.0f && factor <= 1.0f);

		const aiQuaternion &startRotationQ = rotationKeys[rotKeyID].mValue;
		const aiQuaternion &endRotationQ = rotationKeys[nextRotKeyID].mValue;
		aiQuaternion value;
		aiQuaternion::Interpolate(value, startRotationQ, endRotationQ, factor);
		value = value.Normalize();
		return glm::quat(value.w, value.x, value.y, value.z);
	}

	inline glm::vec3 CalcInterpolatedScaling(const aiNodeAnim* pNodeAnim, float animationTime, double animationDuration)
	{
		if (pNodeAnim->mNumScalingKeys == 1) {
			auto &value = pNodeAnim->mScalingKeys[0].mValue;
			return glm::vec3(value.x, value.y, value.z);
		}

		aiVectorKey *scalingKeys = pNodeAnim->mScalingKeys;

		uint keyID = FindScalingKeyID(animationTime, pNodeAnim);
		uint nextKeyID = GetNextAnimationKey(keyID, pNodeAnim->mNumScalingKeys);

		float keyDeltaTime = 0;
		if (nextKeyID == 0) {
			keyDeltaTime = float(animationDuration - scalingKeys[keyID].mTime + scalingKeys[0].mTime);
		}
		else {
			keyDeltaTime = float(scalingKeys[nextKeyID].mTime - scalingKeys[keyID].mTime);
		}

		float factor = (animationTime - (float)scalingKeys[keyID].mTime) / keyDeltaTime;

		auto value = lerp(scalingKeys[keyID].mValue, scalingKeys[nextKeyID].mValue, factor);
		return glm::vec3(value.x, value.y, value.z);
	}
}
