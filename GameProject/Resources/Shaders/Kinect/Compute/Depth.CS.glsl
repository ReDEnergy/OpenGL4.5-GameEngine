#version 430

struct particle {
	vec4 position;
	vec4 stability;
};

layout(binding=0) buffer particles {
		particle data[];
};

layout(binding = 0) uniform sampler2D kinectDepth;
layout(binding = 1, r16f) writeonly uniform image2D depthFilter;
layout(local_size_x = 16, local_size_y = 16) in;

int index;
ivec2 pixel;

float getDepthOffset(ivec2 offset) {
	return texelFetch(kinectDepth, pixel + offset, 0).x;
}

float filterDepth() {
	float avg = 0;
	int in_freq = 0;
	int out_freq = 0;
	int innerBandThreshold = 5;
	int outerBandThreshold = 10;
	
	for (int i=-2; i<=2; i++) {
		for (int j=-2; j<=2; j++) {
			if (i!=0 && j!=0) {
				float d = getDepthOffset(ivec2(i, j)) * 256 * 4;
				if (d < 1) {
					if (abs(i) == 1 || abs(j) == 1)
						in_freq +=1;
					else
						out_freq += 1;
					avg += d;
				}
			}
		}
	}
	
	if (in_freq > innerBandThreshold || out_freq > outerBandThreshold) {
		return avg / (in_freq + out_freq);
	}
	
	return 0;
}

void main(){
	pixel = ivec2(gl_GlobalInvocationID.xy);
	index = pixel.x + pixel.y * 512;
	vec4 color = texelFetch(kinectDepth, pixel, 0);
	float depth = color.x * 256 * 4.5;
	float filtered = filterDepth();
	if (filtered > 0) depth = filtered;

	if (depth < 1) {
		color.x = 0;
		depth = 0;
	}

	// Flip the image horizontal and vertical
	pixel.y = 424 - pixel.y;
	pixel.x = 512 - pixel.x;
	
	// Get previous position
	vec3 tpos = data[index].position.xyz;
	float stability = data[index].position.a;
	// Compute actual position
	vec3 pos = vec3(pixel * 0.2, depth);

	// Simple motion detection
	if (distance(tpos, pos) > 0.5) {
		stability = 0;
	} 
	else {
		stability += 0.5;
		// Temporal z-depth smoothing + motion
		pos = (tpos * stability + pos) / (stability + 1);
	}
	
	// Save new data into buffer
	data[index].position = vec4(pos, stability);

	// Update depth image
	imageStore(depthFilter, pixel, color);
}
