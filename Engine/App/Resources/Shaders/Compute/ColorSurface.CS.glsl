#version 430

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

	uint index = uint(color.z * 5) * 256 + uint(color.y * 15) * 16 + uint(color.x * 15);
	imageStore(colorBuffer, pixel, vec4(0));
	imageStore(colorBuffer, ivec2(index / 64, index % 64), vec4(1));
	atomicAdd(counter[index], 1);
}
