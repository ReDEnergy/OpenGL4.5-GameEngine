#version 310 es
precision highp float;

layout(std430, binding = 0) buffer SSBO {
	uint counter[];
};

layout (binding = 0) uniform sampler2D image;
layout (binding = 1, r16f) uniform image2D colorBuffer;
layout(local_size_x = 16, local_size_y = 16) in;

void main()
{
	ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
	vec4 color = texelFetch(image, pixel, 0);

	uint index = uint(color.z * 5.0) * 256 + uint(color.y * 15.0) * 16 + uint(color.x * 15.0);
	atomicAdd(counter[index], 1);
}
