#version 430
layout(local_size_x = 32, local_size_y = 32) in;

layout(binding = 0, r8) uniform image2D colorBuffer;
layout(binding = 1, rgba32f) uniform readonly image2D viewPosition;
layout(binding = 2, rgba32f) uniform readonly image2D colorMap;

layout(std430, binding = 0) buffer ssbo {
	ivec4 center[];
};

uniform vec3 plane_direction;
uniform vec3 plane_origin;		// relative to camera
uniform float sphere_size;

void main()
{
	ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
	vec3 pos = imageLoad(viewPosition, pixel).xyz;

	float outColor = 0;

	// Plane Intersection
	// float dist = abs(dot(pos.xyz - plane_origin, plane_direction));
	// if (dist < 0.01 )
		// outColor = 1.0;

	// Spehere intersection
	float dist = length(pos);
	if (abs(dist - sphere_size) < 0.01 )
		outColor = 1.0;

	// Save visual information
	imageStore(colorBuffer, pixel, vec4(outColor, 0, 0, 1.0));

	// Save position information
	if (outColor == 1.0)
	{
		vec4 color = imageLoad(colorMap, pixel);
		uint index = uint(color.z * 5) * 256 + uint(color.y * 15) * 16 + uint(color.x * 15);
		atomicAdd(center[index].x, int(pos.x * 1000));
		atomicAdd(center[index].y, int(pos.y * 1000));
		atomicAdd(center[index].z, int(pos.z * 1000));
		atomicAdd(center[index].w, 1);
	}
}
