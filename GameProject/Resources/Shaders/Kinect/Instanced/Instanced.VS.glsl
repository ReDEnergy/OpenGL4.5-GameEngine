#version 430

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec2 v_texture_coord;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform sampler2D u_texture_1;

struct particle {
	vec4 position;
	vec4 stability;
};

layout(std430, binding=0) buffer particles {
		particle data[];
};

layout(location = 0) out vec2 texture_coord;

void main()
{
	int dw = 512;
	int dh = 424;
	int y = int(gl_InstanceID / dw);
	int x = gl_InstanceID - y * dw;

	ivec2 pixel = ivec2(x, y);
	float z = texelFetch(u_texture_1, pixel, 0).x;
	float depth = z * 256 * 4.5;

	// hide errors
	if (depth < 0.1) { 
		depth = 100;
		pixel.y += 500;
	}

	// reverse horizontal
	pixel.y = dh - pixel.y;

	vec3 tpos = data[gl_InstanceID].position.xyz;
	vec3 pos = vec3(pixel * 0.2, - depth);
	float stability = data[gl_InstanceID].stability.x;

	float recover = 0.5;
	
	if (distance(tpos, pos) > 0.5) {
		data[gl_InstanceID].stability.x = 0;
	} else {
		pos = (tpos * stability + pos) / (stability + 1);
	}	

	data[gl_InstanceID].position.xyz = pos;
	data[gl_InstanceID].stability.x += 2;

	texture_coord = v_texture_coord;
	gl_Position = Projection * View * Model * vec4(v_position + tpos, 1.0);
}
