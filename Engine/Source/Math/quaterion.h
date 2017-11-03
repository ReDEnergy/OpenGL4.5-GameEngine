#include <include/glm_utils.h>


glm::quat ClosestQuat1Axis(const glm::quat &rot, int axis)
{
	glm::quat q(0, 0, 0, 0);

	float sqrLenght = rot.w * rot.w + rot[axis] * rot[axis];
	if (sqrLenght) {
		float invLenght = 1 / sqrt(sqrLenght);
		q.w = rot.w * invLenght;
		q[axis] = rot[axis] * invLenght;
	}
	else {
		q.w = 1;
		q[axis] = 0;
	}
	return q;
}

glm::quat ClosestQuatX(const glm::quat &rot)
{
	return ClosestQuat1Axis(rot, 0);
}

glm::quat ClosestQuatY(const glm::quat &rot)
{
	return ClosestQuat1Axis(rot, 1);
}

glm::quat ClosestQuatZ(const glm::quat &rot)
{
	return ClosestQuat1Axis(rot, 2);
}

glm::quat ClosestQuatXY(const glm::quat &rot)
{
	float det = rot.w * rot.z - rot.x * rot.y;

	if (abs(det) < 0.5)
	{
		float discr = sqrt(1 - 4 * det * det);
		float a = rot.w * rot.x - rot.y * rot.z;
		float b = rot.w * rot.w - rot.x * rot.x + rot.y * rot.y - rot.z * rot.z;
		float c0, s0, c1, s1, invLength;

		if (b >= 0)
		{
			c0 = 0.5f * (discr + b);
			s0 = a;
		}
		else
		{
			c0 = a;
			s0 = 0.5f * (discr - b);
		}
		invLength = 1 / sqrt(c0 * c0 + s0 * s0);
		c0 *= invLength;
		s0 *= invLength;

		c1 = rot.w * c0 + rot.x * s0;
		s1 = rot.y * c0 + rot.z * s0;
		invLength = 1 / sqrt(c1 * c1 + s1 * s1);
		c1 *= invLength;
		s1 *= invLength;

		return glm::quat(c0 * c1, s0 * c1, c0 * s1, s0 * s1);
	}

	
	float invLength = 1 / sqrt(abs(det));

	return glm::quat(rot.w * invLength, rot.x * invLength, 0, 0);
}

glm::quat ClosestQuatYZ(const glm::quat &rot)
{
	auto q = ClosestQuatXY(glm::quat(rot.w, rot.y, rot.z, rot.x));
	return glm::quat(q.w, q.z, q.x, q.y);
}

glm::quat ClosestQuatZX(const glm::quat &rot)
{
	auto q = ClosestQuatXY(glm::quat(rot.w, rot.x, rot.z, rot.y));
	return glm::quat(q.w, q.x, q.z, q.y);
}