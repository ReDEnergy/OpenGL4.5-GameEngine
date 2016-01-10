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

#define RADIANS(a)		a * 0.0174532925194444		// a * M_PI / 180
#define TO_RADIANS		0.0174532925194444f			// M_PI / 180
#define TO_DEGREES		57.29577951308233f			// 180 / M_PI
