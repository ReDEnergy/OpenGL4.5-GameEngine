#version 430

layout(std430, binding = 0) buffer viewRayBuffer {
	vec4 cloud[];
};

layout(local_size_x = 16, local_size_y = 16) in;

void main()
{
	cloud[gl_GlobalInvocationID.x] = vec4(0);
}
