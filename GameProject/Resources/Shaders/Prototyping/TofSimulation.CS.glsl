#version 430
layout(local_size_x = 32, local_size_y = 32) in;

uniform sampler2D u_texture_0;
layout(binding = 0, rgba32f) readonly uniform image2D ViewDepth;
layout(binding = 1, r32f) writeonly uniform image2D ToFDepth;

ivec2 pixel;
uniform ivec2 resolution;

void main()
{
	// ------------------------------------------------------------------------
	// Compute array invocation ID

	pixel = ivec2(gl_GlobalInvocationID.xy);

	vec4 depth = texelFetch(u_texture_0, pixel, 0);
	depth.x = pow(depth.x, pow(2, 12));
	// vec4 depth = imageLoad(ViewDepth, pixel);
	
	// ------------------------------------------------------------------------
	// Update depth image
	imageStore(ToFDepth, pixel, depth);
}
