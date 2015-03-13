#version 430

layout (binding = 0) uniform sampler2D image;
layout (binding = 1, rgba16f) writeonly uniform image2D colorBuffer;

layout(local_size_x = 16, local_size_y = 16) in;

vec4 blur(ivec2 pixel, int size) {
	int hsize = size / 2;
	vec4 result = vec4(0.0, 0.0, 0.0, 1.0);
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			ivec2 loc = pixel + ivec2(i, j) - ivec2(hsize);
			vec4 color = texelFetch(image, loc, 0);
			result += color;
		}
	}
	result /= size * size;
	return result;
}

void main()
{
	ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
	vec4 result = blur(pixel, 4);
	imageStore(colorBuffer, pixel, result);
}
