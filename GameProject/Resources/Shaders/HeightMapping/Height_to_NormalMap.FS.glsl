#version 410

#define HeightMap u_texture_0

layout(location = 0) in vec2 texture_coord;

// Textures
uniform sampler2D u_texture_0;

layout(location = 0) out vec3 normal_map;

void main() {
	vec4 texel = texture(HeightMap, texture_coord);
	
	const ivec3 offset = ivec3(-1, 0, 1);

	float center = texture(HeightMap, texture_coord).x;
	
    float left = textureOffset(HeightMap, texture_coord, offset.xy).x;
    float right = textureOffset(HeightMap, texture_coord, offset.zy).x;
    float down = textureOffset(HeightMap, texture_coord, offset.yx).x;
    float up = textureOffset(HeightMap, texture_coord, offset.yz).x;

	float alpha = 10;
    vec3 va = normalize(vec3(1.0, alpha * (right - left), 0.0));
    vec3 vb = normalize(vec3(0.0, alpha * (down - up), 1.0));
    vec3 N = normalize(cross(vb, va));

	normal_map = N;
	// normal_map = texel.rgb;
	// normal_map = vec4(alpha  * (down - up));
	// normal_map = vec4(alpha  * (right - left));
	// int x = int(texture_coord.x * 1024) + int(texture_coord.y*1024);
	// normal_map = vec4(x%2, x%2, x%2, 1.0);
}
