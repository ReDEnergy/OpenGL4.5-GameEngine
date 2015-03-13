#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

inline float lerp(float v0, float v1, float t) {
	return v0 + (v1 - v0) * t;
}

#ifndef max
	#define max(a, b) a > b ? a : b
#endif
#ifndef min
	#define min(a, b) a < b ? a : b
#endif

#define UPPER_BOUND(a, b) (a + b - 1) / b
