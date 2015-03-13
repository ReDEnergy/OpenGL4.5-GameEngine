#version 430

uniform mat4 Model;
uniform sampler2D u_texture_1;

struct particle {
	vec4 position;
	vec4 stability;
};

layout(binding=0) buffer particles {
		particle data[];
};

layout(location = 0) out vec4 info;

void main()
{
	vec3 pos = data[gl_VertexID].position.xyz;

	gl_Position = Model * vec4(pos, 1); 
	info.x = gl_VertexID;
}
