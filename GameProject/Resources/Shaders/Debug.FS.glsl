#version 410
layout(location = 0) in vec2 text_coord;

// Debug Textures
uniform sampler2D u_texture_0;	// Diffuse texture
uniform sampler2D u_texture_1;	// World positions
uniform sampler2D u_texture_2;	// World normals
uniform sampler2D u_texture_3;	// View positions
uniform sampler2D u_texture_4;	// View normals
uniform sampler2D u_texture_5;	// Deferred light
uniform sampler2D u_texture_6;	// Depth buffer
uniform sampler2D u_texture_7;	// Debug Objects
uniform sampler2D u_texture_8;	// Debug Objects
uniform sampler2D u_texture_9;	// Debug Objects
uniform sampler2D u_texture_10;	// Debug Objects
uniform sampler2D u_texture_11;	// Debug Objects
uniform sampler2D u_texture_12;	// Debug Objects
uniform sampler2D u_texture_13;	// Debug Objects
uniform sampler2D u_texture_14;	// Debug Objects
uniform sampler2D u_texture_15;	// Debug Objects

uniform samplerCube u_texture_cube_14;	// Debug Objects

layout(location = 0) out vec4 frag_color;

uniform int debug_id;
uniform vec3 eye_position;
uniform float zFar;
uniform float zNear;

///////////////////////////////////////////////////////////////////////////////
// Utils

float linearDepth(sampler2D depthTexture, vec2 coord);
vec4 viewDepth(sampler2D depthTexture, vec2 coord);
vec4 component(sampler2D textureID, vec2 coord, int channel);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

vec4 CSM() {
	vec4 color = texture(u_texture_0, text_coord);
	float dist = linearDepth(u_texture_6, text_coord);
	if (dist < 2.6)
		return color + vec4(0.5, 0.0, 0.0, 0.0);
	if (dist < 6.5)
		return color + vec4(0.0, 0.5, 0.0, 0.0);
	if (dist < 16)
		return color + vec4(0.0, 0.0, 0.5, 0.0);
	if (dist < 40)
		return color + vec4(0.5, 0.5, 0.0, 0.0);
	if (dist < 100)
		return color + vec4(0.5, 0.0, 0.5, 0.0);
	if (dist < 499)
		return color + vec4(0.0, 0.5, 0.5, 0.0);
	return color;
}

void main() {

	switch(debug_id) {
		case 0:
			frag_color = viewDepth(u_texture_0, text_coord);
			break;
		case 1:
			frag_color = texture(u_texture_1, text_coord);
			break;
		case 2:
			frag_color = texture(u_texture_2, text_coord);
			break;
		case 3:
			frag_color = texture(u_texture_3, text_coord);
			break;
		case 4:
			frag_color = texture(u_texture_4, text_coord);
			break;
		case 5:
			frag_color = texture(u_texture_5, text_coord);
			break;
		case 6:
			frag_color = texture(u_texture_6, text_coord);
			break;
		case 7:
			frag_color = texture(u_texture_7, text_coord);
			break;
		case 8:
			frag_color = texture(u_texture_8, text_coord) * 4500 / 255;
			break;
		case 9:
			frag_color = texture(u_texture_9, text_coord) * 4500 / 255;
			break;
		default:
			frag_color = texture(u_texture_0, text_coord);
	}
	// CubeMapping
	// vec4 wpos = texture(u_texture_1, text_coord);
	// vec3 dir = wpos.xyz - eye_position;
	// frag_color = vec4(dir, 1.0);
}

///////////////////////////////////////////////////////////////////////////////
// Utils

float linearDepth(sampler2D depthTexture, vec2 coord) {
	float zt = texture(depthTexture, coord).x;
	float zn = 2.0 * zt - 1.0;
	float z = (2.0 * zNear * zFar) / (zFar + zNear - zn * (zFar - zNear));
	return z;
}

vec4 viewDepth(sampler2D depthTexture, vec2 coord) {
	float dist = texture(depthTexture, coord).x;
	float d = pow(dist , zFar);
	// float d = pow(dist , pow(2, 12));
	return vec4(d, d, d, 1.0);
}

vec4 component(sampler2D textureID, vec2 coord, int channel) {
	float color = texture(textureID, coord)[channel];
	return vec4(vec3(color), 1.0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
