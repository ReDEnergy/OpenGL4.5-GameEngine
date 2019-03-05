#version 410

#define Diffuse	u_texture_0
uniform sampler2D u_texture_0;	// Diffuse texture
uniform vec2 resolution;

layout(location = 0) out vec4 magnitude;	// Sobel gradient magnitude
layout(location = 1) out vec4 direction;	// Sobel gradient direction

vec4 getSobel(sampler2D u_texture, vec2 text_coords);

void main() {
	vec2 text_coord = gl_FragCoord.xy / resolution;
	// vec4 color = texture(Diffuse, text_coord);
	vec4 color = getSobel(Diffuse, text_coord);
	magnitude = color;
}

// -----------------------------------------------------------------------------
// Texture Sampling

vec2 texel_offset = vec2(1.0/resolution);
vec4 getSample(sampler2D u_texture, vec2 text_coords, vec2 offset) {
	vec4 color = texture(u_texture, text_coords + offset * texel_offset);
	return color;
}

// -----------------------------------------------------------------------------
// Sobel Filter

const mat3 SDx = mat3(
	-1.0, 0.0, 1.0,
	-2.0, 0.0, 2.0,
	-1.0, 0.0, 1.0
);

const mat3 SDy = mat3(
	 1.0,  2.0,  1.0,
	 0.0,  0.0,  0.0,
	-1.0, -2.0, -1.0
);

vec3 samples[3][3];

int getDirection(float Dx, float Dy) {
	if (Dy == 0.0)	return 0;
	if (Dx == 0.0)	return 90;

	float theta = abs(atan(Dy/Dx));
	if (theta < 0)
		theta+= 180;
	if (theta > 0 && theta < 22.5)
		return 0;
	if (theta >= 22.5 && theta < 67.5)
		return 45;
	if (theta >= 67.5 && theta < 112.5)
		return 90;
	if (theta >= 112.5 && theta < 157.5)
		return 135;
	if (theta >= 157.5 && theta <= 180.0)
		return 0;
}

vec3 getDirections(vec3 Dx, vec3 Dy) {
	int r = getDirection(Dx.r, Dy.r);
	int g = getDirection(Dx.g, Dy.g);
	int b = getDirection(Dx.b, Dy.b);
	return vec3(r, g, b);
}

vec4 getSobel(sampler2D u_texture, vec2 text_coords) {
	int x, y; 
	vec3 Dx = vec3(0.0);
	vec3 Dy = vec3(0.0);
	vec3 color = vec3(0.0); // gradient magnitude
	for (y = 0; y < 3; y += 1) {
		for (x = 0; x < 3; x += 1) {
			samples[y][x] = getSample(u_texture, text_coords, vec2(x - 1, y - 1)).rgb;
			if (SDx[y][x])
				Dx += SDx[y][x] * samples[y][x];
			if (SDy[y][x])
				Dy += SDy[y][x] * samples[y][x];
		}
	}
	color = abs(Dx) + abs(Dy);
	direction = vec4(getDirections(Dx, Dy), 1.0);
	
	return vec4(color, 1.0);
}