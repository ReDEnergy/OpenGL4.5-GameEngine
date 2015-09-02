#pragma once
#include <include/gl.h>
#include <include/glm.h>
#include <include/math.h>

#include <sstream>

typedef unsigned char uchar;
using namespace std;

namespace glm {

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
		int s =  (int)sign((b.x - a.x)*(c.z - a.z) - (b.z - a.z)*(c.x - a.x));
		return (s > 0) ? 1 : -1;
	}

	inline glm::quat axis_angle(const float xx, const float yy, const float zz, const float angle)
	{
		float t = float(RADIANS(angle) / 2.0);

		// Calculate the sin( theta / 2) once for optimization
		float result = sin(t);

		// Calculate the x, y and z of the quaternion
		float x = xx * result;
		float y = yy * result;
		float z = zz * result;

		// Calcualte the w value by cos( theta / 2 )
		float w = cos(t);

		return glm::quat(w, x, y, z);
	}

	// Rotate a Point around OY (0, 1, 0) with a specific angle(radians)
	inline glm::vec3 RotateOY(const glm::vec3 P, float radians) {
		glm::vec3 R;
		R.x = P.x * cos(radians) - P.z * sin(radians);
		R.y = P.y;
		R.z = P.x * sin(radians) + P.z * cos(radians);
		return R;
	}

	template<class T>
	istream& operator>>(istream& is, T& vec) {
		size_t i = 0;
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

	inline void BindUniform4f(GLint location, glm::vec4 vec) {
		glUniform4f(location, vec.x, vec.y, vec.z, vec.t);
	};

	inline void BindUniform3f(GLint location, glm::vec3 vec) {
		glUniform3f(location, vec.x, vec.y, vec.z);
	};
}

/**
 ** overload ostream << for easy printing glm classes
 */
inline ostream &operator<< (ostream &out, const glm::ivec2 &vec) {
	out << "{"
		<< vec.x << " " << vec.y
		<< "}";

	return out;
}

inline ostream &operator<< (ostream &out, const glm::ivec3 &vec) {
	out << "{"
		<< vec.x << " " << vec.y << " " << vec.z
		<< "}";

	return out;
}

inline ostream &operator<< (ostream &out, const glm::ivec4 &vec) {
	out << "{"
		<< vec.x << " " << vec.y << " " << vec.z << " " << vec.w
		<< "}";

	return out;
}

inline ostream &operator<< (ostream &out, const glm::vec2 &vec) {
	out << "{"
		<< vec.x << " " << vec.y
		<< "}";

	return out;
}

inline ostream &operator<< (ostream &out, const glm::vec3 &vec) {
	out << "{"
		<< vec.x << " " << vec.y << " " << vec.z
		<< "}";

	return out;
}

inline ostream &operator<< (ostream &out, const glm::vec4 &vec) {
	out << "{"
		<< vec.x << " " << vec.y << " " << vec.z << " " << vec.w
		<< "}";

	return out;
}

inline ostream &operator<< (ostream &out, const glm::quat &rot) {
	out << "{"
		<< rot.x << " " << rot.y << " " << rot.z << " " << rot.w
		<< "}";

	return out;
}
