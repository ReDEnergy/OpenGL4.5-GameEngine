#version 430

layout (binding = 0) uniform sampler2D image;
layout (binding = 1, rgba16f) writeonly uniform image2D colorBuffer;

layout(local_size_x = 16, local_size_y = 16) in;

vec4 vec_pow(vec4 v, float power) {
	v.r = pow(v.r, power);
	v.g = pow(v.g, power);
	v.b = pow(v.b, power);
	return v;
}

vec4 arithmetic_mean(ivec2 pixel, ivec2 kernel) {
	ivec2 hsize = kernel / 2;
	vec4 result = vec4(0.0, 0.0, 0.0, 1.0);

	for (int i = 0; i < kernel.x; i++) {
		for (int j = 0; j < kernel.y; j++) {
			ivec2 loc = pixel + ivec2(i, j) - hsize;
			vec4 color = texelFetch(image, loc, 0);
			result += color;
		}
	}
	result /= kernel.x * kernel.y;
	return result;
}

vec4 geometric_mean(ivec2 pixel, ivec2 kernel) {
	ivec2 hsize = kernel / 2;
	vec4 result = vec4(1.0);

	for (int i = 0; i < kernel.x; i++) {
		for (int j = 0; j < kernel.y; j++) {
			ivec2 loc = pixel + ivec2(i, j) - hsize;
			vec4 color = texelFetch(image, loc, 0);
			result *= color;
		}
	}
	float exp = 1.0 / (kernel.x * kernel.y);
	result.r = pow(result.r, exp);
	result.g = pow(result.g, exp);
	result.b = pow(result.b, exp);
	return result;
}

vec4 contraharmonic_mean(ivec2 pixel, ivec2 kernel, float Q) {
	ivec2 hsize = kernel / 2;
	vec4 result0 = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 result1 = vec4(0.0, 0.0, 0.0, 1.0);

	for (int i = 0; i < kernel.x; i++) {
		for (int j = 0; j < kernel.y; j++) {
			ivec2 loc = pixel + ivec2(i, j) - hsize;
			vec4 color = texelFetch(image, loc, 0);
			result0 += vec_pow(color, Q + 1);
			result1 += vec_pow(color, Q);
		}
	}
	return result0 / result1;
}


vec4 median(ivec2 pixel, ivec2 kernel) {
	ivec2 hsize = kernel / 2;
	float values[256];
	int count = kernel.x * kernel.y;
	for (int i = 0; i < kernel.x; i++) {
		for (int j = 0; j < kernel.y; j++) {
			ivec2 loc = pixel + ivec2(i, j) - hsize;
			vec4 color = texelFetch(image, loc, 0);
			float grey = color.r;
			values[i * kernel.y + j] = grey;
		}
	}
	for (int i = 0; i < count; i++) {
		for (int j = i; j < (count - 1); j++) {
			if (values[j] < values[j+1]) {
				float x = values[j];
				values[j] = values[j+1];
				values[j+1] = x;
			}
		}
	}
	float m = values[int(count/2)];
	return vec4(m, m, m, 1.0);
}

vec4 region(ivec2 topleft, ivec2 bottomright) {
	ivec2 diff = bottomright - topleft;
	vec4 result = vec4(0.0, 0.0, 0.0, 1.0);
	float min = 0;
	float max = 0;
	for (int i = 0; i < diff.x; i++) {
		for (int j = 0; j < diff.y; j++) {
			ivec2 loc = topleft + ivec2(i, j);
			vec4 color = texelFetch(image, loc, 0);
			result += color;
		}
	}
	result /= (diff.x * diff.y);
	return result;
}

vec4 kuwahara(ivec2 pixel, int size) {
	int hsize = size / 2;
	int region_size = hsize + 1;
	float med[4];

	// top left
	med[0] = region(pixel - ivec2(hsize), pixel).r;
	// top right
	med[1] = region(pixel - (0, hsize), pixel + (hsize, 0)).r;
	// bottom right
	med[2] = region(pixel, pixel + ivec2(hsize)).r;
	// bottom left
	med[3] = region(pixel - (hsize, 0), pixel + (0, hsize)).r;
	
	float m = med[0];
	return vec4(m, m, m, 1.0);
}

void main(){
	ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
	vec4 result = arithmetic_mean(pixel, ivec2(3, 3));
	// vec4 result = geometric_mean(pixel, ivec2(3, 3));
	// vec4 result = contraharmonic_mean(pixel, ivec2(3, 3), 1.5);
	// vec4 result = contraharmonic_mean(pixel, ivec2(3, 3), 1.5);
	// vec4 result = median(pixel, ivec2(3, 3));
	// vec4 result = kuwahara(pixel, 13);
	// vec4 result = texelFetch(image, pixel, 0);

	// output
	result = vec4(vec3(result.x), 1.0);
	imageStore(colorBuffer, pixel, result);
}
