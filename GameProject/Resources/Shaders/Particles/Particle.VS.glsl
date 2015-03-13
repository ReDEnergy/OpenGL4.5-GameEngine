#version 430

uniform mat4 Model;
uniform sampler2D u_texture_1;

struct particle {
	vec4 position;
	vec4 stability;
};

layout(std430, binding=0) buffer particles {
		particle data[];
};

float rand(vec2 co){;
	return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
	int dw = 512;
	int dh = 424;
	int y = int(gl_VertexID / dw);
	int x = gl_VertexID - y * dw;

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

	vec3 tpos = data[gl_VertexID].position.xyz;
	vec3 pos = vec3(pixel * 0.2, -depth);
	float stability = data[gl_VertexID].stability.x;

	if (distance(tpos, pos) > 0.5) {
		data[gl_VertexID].stability.x = 0;
	} else {
		pos = (tpos * stability + pos) / (stability + 1);
	}	

	data[gl_VertexID].position.xyz = pos;
	data[gl_VertexID].stability.x += 1;
	gl_Position = Model * vec4(tpos, 1); 
}
