#version 410
#define HeightMap	u_texture_0
#define TerrainN	u_texture_1

layout(location = 0) in vec2 texture_coord;
layout(location = 1) in vec3 world_position;

uniform sampler2D u_texture_0;
uniform sampler2D u_texture_1;

layout(location = 0) out vec4 frag_color;

// ligthing attribute
uniform vec3 eye_position;
uniform vec3 light_position;

vec4 PhongLight(vec3 w_pos, vec3 w_N);
vec4 CalcSpotLight(vec3 w_pos, vec3 w_N);

void main()
{
	vec4 diffuse = texture(HeightMap, texture_coord);
	vec4 normal = texture(TerrainN, texture_coord);

	vec4 light_color = PhongLight(world_position, normal.xyz);	
	frag_color =  diffuse + light_color;
}

const vec3 kd = vec3 (0.8, 0.8, 0.8);
const vec3 ks = vec3 (0.8, 0.8, 0.8);
const vec3 ld = vec3 (1.0, 1.0, 1.0);
const vec3 ls = vec3 (1.0, 1.0, 1.0);
const vec3 la = vec3 (0.2, 0.2, 0.2);
const float specular_exponent = 40.0;
const vec3 light_dir = vec3 (0.0, -1.0, 0.0);
const float cut_off = 0.75;

vec4 PhongLight(vec3 w_pos, vec3 w_N)
{
	vec3 L = normalize(light_position - w_pos);
	vec3 V = normalize(eye_position - w_pos);
	vec3 H = normalize(L + V);

	float d = distance(light_position, w_pos) / 50;
	float att = 1.0f / pow(d, 2);
	
	float dot_specular = dot(w_N, L);
	vec3 specular = vec3(0);
	if (dot_specular > 0)
		specular = ls * pow(max(dot(w_N, H), 0), specular_exponent);

	vec3 diffuse = ld * max(dot_specular, 0);

	return vec4(att * (diffuse + specular), 1.0);
}

vec4 CalcSpotLight(vec3 w_pos, vec3 w_N)
{
	vec3 light_direction  = normalize(w_pos - light_position);
	float spot_factor = dot(light_direction, light_dir);

	if (spot_factor > cut_off) {
		vec4 color = PhongLight(w_pos, w_N);
		return color * (1.0 - (1.0 - spot_factor) / (1.0 - cut_off));
	}
	return vec4(0, 0, 0, 0);
} 