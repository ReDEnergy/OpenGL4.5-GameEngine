#version 310 es
#define DepthT	u_texture_1

layout(location = 0) in vec2 texture_coord;
layout(location = 1) in vec3 world_position;
layout(location = 2) in vec3 world_normal;
layout(location = 3) in vec4 shadow_pos;

uniform sampler2D u_texture_0;	// Diffuse texture
uniform sampler2D u_texture_1;	// Sun Depth buffer

uniform vec2 shadow_texel_size;
uniform vec3 eye_position;

layout(location = 0) out vec3 out_diffuse;
layout(location = 1) out vec3 out_world_position;
layout(location = 2) out vec3 out_world_normal;

float EvalShadow();
vec4 PhongLight(vec3 w_pos, vec3 w_N);
vec4 shp;

void main() {
	vec4 diffuse = texture(u_texture_0, texture_coord);
	if(diffuse.a < 0.9)
		discard;

	////////////////////////////////
	// Shadow Mapping START

	// NDC to Screen coordinates
	shp = shadow_pos / shadow_pos.w ;
	shp = shp * 0.5 + 0.5;
	
	float shadow = EvalShadow();
	out_diffuse *= shadow;
	
	// Shadow Mapping END
	//////////////////////////////
}

// -----------------------------------------------------------------------------
// Percentage Closer Filtering (PCF) -------------------------------------------

int IsLit(vec2 offset) {
	float bias = 0.00005;
	float depth = texture(u_texture_1, shp.xy + offset * shadow_texel_size).x;
	if (depth + bias < shp.z)
		return 0;
	return 1;
}

float PCFfactor() {
	float x, y;
	float sum = 0;
	float kernel_size = 4;
	float limit = kernel_size/2 - 0.5;
	
	sum += IsLit(vec2( 0.0, 0.0));
	sum += IsLit(vec2(-limit, -limit));
	sum += IsLit(vec2(-limit,  limit));
	sum += IsLit(vec2( limit, -limit));
	sum += IsLit(vec2( limit,  limit));

	if (sum == 0)	return 0;
	if (sum == 5)	return 1;

	sum -= IsLit(vec2( 0.0, 0.0));
	
	for (y = -limit; y <= limit; y += 1.0) {
		for (x = -limit; x <= limit; x += 1.0) {
			if (abs(x) == abs(y) && abs(x) == limit)
				continue;
			sum += IsLit(vec2(x, y));
		}
	}

	return sum / pow(kernel_size, 2);
}

float EvalShadow() {
	if (IsLit(vec2(0.0, 0.0)) == 0)
		return 0.6;
	return mix(0.6, 1.0, pow(PCFfactor(), 1.6));
}