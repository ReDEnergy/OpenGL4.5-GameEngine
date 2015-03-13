#version 410

#define Magnitude	u_texture_0
#define Direction	u_texture_1

uniform sampler2D u_texture_0;	// Sobel gradient magnitude
uniform sampler2D u_texture_1;	// Sobel gradient direction
uniform vec2 resolution;

layout(location = 0) out vec4 frag_color;

vec4 getNonMaxSurp();
vec4 HystThresholding(vec4 color);
vec2 text_coords;

void main() {
	text_coords = gl_FragCoord.xy / resolution;
	// vec4 diffuse = texture(Magnitude, text_coords);
	vec4 diffuse = getNonMaxSurp();
	diffuse = HystThresholding(diffuse);
	frag_color = diffuse;
}

// -----------------------------------------------------------------------------
// Texture Sampling

vec2 texel_offset = vec2(1.0/resolution);
vec4 getSample(sampler2D u_texture, vec2 text_coords, vec2 offset) {
	vec4 color = texture(u_texture, text_coords + offset * texel_offset);
	return color;
}

// -----------------------------------------------------------------------------
// Non-Maximum Surpression

vec4 magnitude, s1, s2;

void getNPS(float theta) {
	if (theta == 0) {
		s1 = getSample(Magnitude, text_coords, vec2(-1.0, 0.0));
		s2 = getSample(Magnitude, text_coords, vec2( 1.0, 0.0));
	}
	if (theta == 45) {
		s1 = getSample(Magnitude, text_coords, vec2( 1.0,  1.0));
		s2 = getSample(Magnitude, text_coords, vec2(-1.0, -1.0));
	}
	if (theta == 90) {
		s1 = getSample(Magnitude, text_coords, vec2( 0.0,  1.0));
		s2 = getSample(Magnitude, text_coords, vec2( 0.0, -1.0));
	}
	if (theta == 135) {
		s1 = getSample(Magnitude, text_coords, vec2(-1.0,  1.0));
		s2 = getSample(Magnitude, text_coords, vec2( 1.0, -1.0));
	}
}

vec4 getNonMaxSurp() {
	vec4 color = vec4(0.0);
	vec4 theta = texture(Direction, text_coords);
	magnitude = texture(Magnitude, text_coords);

	getNPS(theta.r);
	if (magnitude.r >= s1.r && magnitude.r >= s2.r)
		color.r = magnitude.r;
		
	getNPS(theta.g);
	if (magnitude.g >= s1.g && magnitude.g >= s2.g)
		color.g = magnitude.g;

	getNPS(theta.b);
	if (magnitude.b >= s1.b && magnitude.b >= s2.b)
		color.b = magnitude.b;
		
	return color;
}

// -----------------------------------------------------------------------------
// Hysteresis Thresholding

float Tmin = 80.0/255;
float Tmax = 130.0/255;

vec4 HystThresholding(vec4 color) {
	if (color.r < Tmin)
		return vec4(0.0);
	if (color.r > Tmax)
		return vec4(1.0);

	int i, j;
	float mag = 0;
	for (i=-1; i<=1; i++) {
		for (j=-1; j<=1; j++) {
			if (i!=0 && j!=0) {
				mag = getSample(Magnitude, text_coords, vec2(i, j)).r;
				if (mag > Tmax)
					return vec4(1.0);
			}
		}
	}
	return vec4(1.0, 1.0, 0.0, 1.0);
}

