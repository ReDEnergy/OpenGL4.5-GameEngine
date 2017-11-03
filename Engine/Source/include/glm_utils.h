#pragma once
#include <include/glm.h>
#include <include/math.h>

#include <sstream>

typedef unsigned char uchar;

namespace glm
{
	using namespace std;

	const glm::vec3 vec3_up		= glm::vec3( 0.0f,  1.0f,  0.0f);
	const glm::vec3 vec3_down	= glm::vec3( 0.0f, -1.0f,  0.0f);
	const glm::vec3 vec3_left	= glm::vec3(-1.0f,  0.0f,  0.0f);
	const glm::vec3 vec3_right	= glm::vec3( 1.0f,  0.0f,  0.0f);
	const glm::vec3 vec3_forward = glm::vec3( 0.0f,  0.0f, 1.0f);
	const glm::vec3 vec3_backward = glm::vec3( 0.0f,  0.0f,  -1.0f);

	const glm::mat4 matI4 = glm::mat4(1.0f);
	const glm::mat3 matI3 = glm::mat3(1.0f);

	inline int GetSideOfPointToLine(glm::vec3 a, glm::vec3 b, glm::vec3 c)
	{
		int s = (int)sign((b.x - a.x)*(c.z - a.z) - (b.z - a.z)*(c.x - a.x));
		return (s > 0) ? 1 : -1;
	}

	inline glm::quat AxisAngle(const float xx, const float yy, const float zz, const float angle360)
	{
		float t = float(RADIANS(angle360) / 2.0);

		// Precompute the sin( theta / 2) once for optimization
		float sinT = sin(t);

		// Calculate the x, y and z of the quaternion
		float x = xx * sinT;
		float y = yy * sinT;
		float z = zz * sinT;
		float w = cos(t);		// W value by cos( theta / 2 )

		return glm::quat(w, x, y, z);
	}

	// Convert a quaternion to axis angle
	inline glm::vec4 GetAxisAngle(glm::quat &rotation, int precision = 0)
	{
		float angle = acos(rotation.w);

		if (angle == 0) {
			return glm::vec4(1, 0, 0, 0);
		}

		auto t = sqrt(1 - rotation.w * rotation.w);

		angle = static_cast<float>(angle * 180 / M_PI);

		if (precision) {
			auto x = round(rotation.x / t * precision) / precision;
			auto y = round(rotation.y / t * precision) / precision;
			auto z = round(rotation.z / t * precision) / precision;
			return glm::vec4(x, y, z, round(angle));
		}

		return glm::vec4(rotation.x / t, rotation.y / t, rotation.z / t, round(angle));
	}

	// Rotate a Point around OY (0, 1, 0) with a specific angle(radians)
	inline glm::vec3 RotateOY(const glm::vec3 P, float radians) {
		glm::vec3 R;
		R.x = P.x * cos(radians) - P.z * sin(radians);
		R.y = P.y;
		R.z = P.x * sin(radians) + P.z * cos(radians);
		return R;
	}

	// Returns quaternion B in same 3D sphere as quaternion A
	inline glm::quat Same3DSphere(glm::quat a, glm::quat b)
	{
		if (glm::dot(a, b) < 0) {
			return glm::inverse(b);
		}
		return b;
	}

	inline glm::quat EnhancedSlerp(glm::quat a, glm::quat b, float ammount) {
		b = Same3DSphere(a, b);
		return glm::slerp(a, b, ammount);
	}

	inline glm::quat RotationBetweenQuat(glm::quat a, glm::quat b) {
		b = Same3DSphere(a, b);
		return glm::inverse(a) * b;
	}

	inline float QuaternionAngle(glm::quat rotation)
	{
		rotation = glm::normalize(rotation);
		float angle = 2.0f * acos(rotation.w);
		return angle;
	}

	template<class T>
	istream& operator>>(istream& is, T& vec) {
		int i = 0;
		for (; i < vec.length(); ++i) {
			is >> vec[i];
			if (!is) {
				assert(i == 1 && "can only have one value");
				break;
			}
		}
		for (; i < vec.length(); ++i) {
			vec[i] = vec[0];
		}
		return is;
	}


	template <class T>
	T ExtractVector(const char* s) {
		T vec;
		stringstream ss(s);
		ss >> vec;
		return vec;
	}

	template <class T>
	inline T color(uchar red, uchar green, uchar blue, uchar alpha = 255) {
		return T(glm::vec4(red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f));
	}

	template<class T>
	inline string Serialize(const T& value, char separator = ' ') {
		string buffer;
		for (uint i = 0; i < value.length(); i++)
		{
			buffer += to_string(value[i]) + separator;
		}
		return buffer;
	}
}

/**
 ** overload ostream operator << for easy printing glm classes
 */
inline std::ostream &operator<< (std::ostream &out, const glm::ivec2 &vec) {
	out << "[" << vec.x << " " << vec.y << "]";
	return out;
}

inline std::ostream &operator<< (std::ostream &out, const glm::vec2 &vec) {
	out << "[" << vec.x << " " << vec.y << "]";
	return out;
}

inline std::ostream &operator<< (std::ostream &out, const glm::ivec3 &vec) {
	out << "[" << vec.x << " " << vec.y << " " << vec.z << "]";
	return out;
}

inline std::ostream &operator<< (std::ostream &out, const glm::vec3 &vec) {
	out << "[" << vec.x << " " << vec.y << " " << vec.z << "]";
	return out;
}

inline std::ostream &operator<< (std::ostream &out, const glm::ivec4 &vec) {
	out << "[" << vec.x << " " << vec.y << " " << vec.z << " " << vec.w << "]";
	return out;
}

inline std::ostream &operator<< (std::ostream &out, const glm::vec4 &vec) {
	out << "[" << vec.x << " " << vec.y << " " << vec.z << " " << vec.w << "]";
	return out;
}

inline std::ostream &operator<< (std::ostream &out, const glm::quat &rot) {
	out << "[" << rot.x << " " << rot.y << " " << rot.z << " " << rot.w << "]";
	return out;
}

/**
** overload ostream operator >> for easy reading glm values
*/

static char skipChar;

inline std::istream &operator>> (std::istream &in, glm::ivec2 &vec) {
	in >> skipChar >> vec.x >> vec.y >> skipChar;
	return in;
}

inline std::istream &operator>> (std::istream &in, glm::vec2 &vec) {
	in >> skipChar >> vec.x >> vec.y >> skipChar;
	return in;
}

inline std::istream &operator>> (std::istream &in, glm::ivec3 &vec) {
	in >> skipChar >> vec.x >> vec.y >> vec.z >> skipChar;
	return in;
}

inline std::istream &operator>> (std::istream &in, glm::vec3 &vec) {
	in >> skipChar >> vec.x >> vec.y >> vec.z >> skipChar;
	return in;
}

inline std::istream &operator>> (std::istream &in, glm::ivec4 &vec) {
	in >> skipChar >> vec.x >> vec.y >> vec.z >> vec.w >> skipChar;
	return in;
}

inline std::istream &operator>> (std::istream &in, glm::vec4 &vec) {
	in >> skipChar >> vec.x >> vec.y >> vec.z >> vec.w >> skipChar;
	return in;
}

inline std::istream &operator>> (std::istream &in, glm::quat &rot) {
	in >> skipChar >> rot.x >> rot.y >> rot.z >> rot.w >> skipChar;
	return in;
}