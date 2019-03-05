#version 430

uniform sampler2D u_texture_1;

layout(std430, binding = 1) buffer viewRayBuffer {
	vec3 cloud[];
};

uniform mat4 Model;
uniform ivec2 resolution;
uniform int texture_pack_mode;

layout(location = 0) out vec2 cellID;
layout(location = 1) out float depth;

void main()
{
	// Get pixel 
	int y = int(gl_VertexID / resolution.x);
	int x = gl_VertexID - y * resolution.x;
	ivec2 pixel = ivec2(x, y);
	
	// Get depth from texture
	vec4 value = texelFetch(u_texture_1, pixel, 0);	
	depth = uint(value.x * 65535);

	// Get view ray
	vec3 viewRay = vec3(float(pixel.x) / resolution.x, float(pixel.y) / resolution.y, 1.0);
	
	// Compute view space position
	vec3 pos = viewRay * 2;
	pos.z = depth * -pos.z / 1000;
	pos.y = -pos.y;
	pos.x = -pos.x;
	
	// depth information is in millimeters but model is scalled down by 10 (because of the sensorBox)
	// position = position * 10 / 1000;
	float posz = depth / 100;
	float posx = float(x) / resolution.x - 0.5f;
	float posy = 0.5f - float(y) / resolution.y;

	float cpx = 1.12693393f;
	float cpy = 0.845200419f;
	
	pos.x = posx * posz * cpx;
	pos.y = posy * posz * cpy;
	pos.z = -posz;

	int group_color = 0;
	if (group_color == 1) {
		cellID = vec2((pixel.x * 10) / resolution.x / 10.0f, (pixel.y * 10) / resolution.y / 10.0f);
	} else {
		cellID = vec2(float(pixel.x) / resolution.x, float(pixel.y) / resolution.y);
	}
	gl_Position = Model * vec4(pos, 1);
}
