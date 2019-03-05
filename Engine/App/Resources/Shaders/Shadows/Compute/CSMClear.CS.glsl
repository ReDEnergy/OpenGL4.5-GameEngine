#version 430

layout (binding = 0, rgba32f) uniform image2D colorBuffer;

layout(local_size_x = 16, local_size_y = 16) in;
uniform vec4 channel_mask;


void main()
{
	ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
	vec4 color = imageLoad(colorBuffer, pixel);
	vec4 mask = vec4(1.0) - channel_mask;
	vec4 cache = color * mask + channel_mask;
	imageStore(colorBuffer, pixel, cache);
}

