#version 410

#define WorldPos	u_texture_0
#define WorldNorm	u_texture_1

uniform sampler2D u_texture_0;
uniform sampler2D u_texture_1;

uniform ivec2 resolution;
uniform vec3 eye_position;
uniform vec3 light_position;
uniform vec3 light_color;
uniform float light_radius;

layout(location = 0) out vec3 out_color;

vec4 PhongLight(vec3 w_pos, vec3 w_N);
vec4 CalcSpotLight(vec3 w_pos, vec3 w_N);

void main()
{
	vec2 text_coord = gl_FragCoord.xy / resolution;
	vec3 wPos = texture(WorldPos, text_coord).xyz;
	vec3 wNorm = texture(WorldNorm, text_coord).xyz;
	out_color = light_color * PhongLight(wPos, wNorm).xyz;
}

// -----------------------------------------------------------------------------
// Phong Light

const vec3 ld = vec3 (0.5);	// Diffuse factor
const vec3 ls = vec3 (0.3);	// Specular factor
const float specular_exponent = 40.0;	// Specular exponent
const vec3 light_dir = vec3 (0.0, -1.0, 0.0);
const float cut_off = 0.75;
 
vec4 PhongLight(vec3 w_pos, vec3 w_N)
{
	vec3 L = normalize(light_position - w_pos);

	float dist = distance(light_position, w_pos);

	if (dist > light_radius)
		return vec4(0);

	float att = pow(light_radius - dist, 2);
	
	float dot_specular = dot(w_N, L);
	vec3 specular = vec3(0);
	if (dot_specular > 0) {
		vec3 V = normalize(eye_position - w_pos);
		vec3 H = normalize(L + V);
		specular = ls * pow(max(dot(w_N, H), 0), specular_exponent);
	}

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