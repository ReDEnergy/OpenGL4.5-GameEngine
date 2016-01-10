#version 430

struct particle
{
	vec4 position;
	vec4 stability;
};

layout(binding = 0) buffer CloudPoint {
		particle data[];
};

layout(std430, binding = 1) buffer KinectCloudPoint {
		vec3 cloud[];
};

layout(std430, binding = 2) buffer DepthToCloud {
		vec2 depthToCloud[];
};

layout(std430, binding = 3) buffer DepthValues {
		float depthValue[];
};

layout(binding = 0) uniform sampler2D kinectDepth;
layout(binding = 1, r16f) writeonly uniform image2D depthFilter;
layout(local_size_x = 16, local_size_y = 16) in;

int index;
ivec2 pixel;
#define KDepthWidth		512
#define KDepthHeight	412

float getDepthOffset(ivec2 offset) {
	return texelFetch(kinectDepth, pixel + offset, 0).x;
}

// int get16BitDepth(int index) {
	// int s = index & 1;
	// int value = depthValue[index / 2];
	// return (s == 0) ? (value >> 16) : (value & ((1 << 16) - 1));
// }

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

void main()
{
	// ------------------------------------------------------------------------
	// Compute index and get kinect depth info for the current voxel

	pixel = ivec2(gl_GlobalInvocationID.xy);
	index = pixel.x + pixel.y * KDepthWidth;
	vec4 color = texelFetch(kinectDepth, pixel, 0);
	float depth = color.x * 256 * 4.5;
	// depth = depthValue[index] / 60.0f;

	// ------------------------------------------------------------------------
	// Smooth depth values

	// float filtered = filterDepth();
	// if (filtered > 0) depth = filtered;

	// if (depth < 1) {
		// color.x = 0;
		// depth = 0;
	// }

	// ------------------------------------------------------------------------
	// Flip the image horizontal and vertical
	pixel.y = KDepthHeight - pixel.y;
	pixel.x = KDepthWidth - pixel.x;
	
	// ------------------------------------------------------------------------
	// Get previous position
	vec3 previousPos = data[index].position.xyz;
	float stability = data[index].position.a;
	vec3 pos;
	// Compute actual position
	pos = vec3(depth * depthToCloud[index], depth);
	// pos = vec3(pixel * 0.2, depth);8
	// pos = vec3(-cloud[index].x, cloud[index].yz) * 20;


	// ------------------------------------------------------------------------
	// Jitter Reduction
	// if (pixel.x < KDepthWidth / 2 && (pixel.y < KDepthHeight / 3))
	// if (pixel.x < KDepthWidth / 2)
	// {
		// float jitterRadius = 3;
		// float deltaDist = distance(previousPos, pos);
		// if (deltaDist <= jitterRadius) {
			// float alpha = deltaDist / pow(jitterRadius, 1);
			// pos = (pos * alpha) + (previousPos * (1.0f - alpha));
		// }
		// stability = 1;
	// }
	// stability = 1;
	
	// ------------------------------------------------------------------------
	// Double Exponential Filtering
	// if (pixel.x > KDepthWidth / 2) 
	// {
		// float smoothing = 0.25;
		// float correction = 0.25;

		// Compute filtered position
		// vec3 prevTrend = data[index].stability.xyz;
		// pos = (smoothing * pos) + (1 - smoothing) * (previousPos);

		// Compute new trend
		// vec3 deltaPos = pos - previousPos;
		// vec3 trend = (correction * deltaPos) + (1 - correction);
		
		// data[index].stability.xyz = trend;
	// }
	
	// ------------------------------------------------------------------------
	// Simple motion stability
	// Temporal z-depth smoothing + motion
	// if (pixel.x < KDepthWidth / 2 && pixel.y > KDepthHeight / 2)
	// if (pixel.x < KDepthWidth)
	// {	
		// float motionTrigger = 0.5;
		// if (distance(previousPos, pos) > motionTrigger) {
			// stability = 0;
		// } 
		// else {
			// stability += 0.5;
			// pos = (previousPos * stability + pos) / (stability + 1);
		// }
	// }
	
	// ------------------------------------------------------------------------
	// Save new data into buffer
	data[index].position = vec4(pos, stability);
	// data[index].stability = vec4(0, float(pixel.y) / KDepthHeight, 0, 1);
	// data[index].stability = vec4(float(pixel.x) / KDepthWidth, 0, 0, 1);
	data[index].stability = vec4(float(pixel.x) / KDepthWidth, float(pixel.y) / KDepthHeight, pos.z /150.0f , 1);
	// data[index].stability = vec4(float(pixel.x) / KDepthWidth, float(pixel.y) / KDepthHeight, 0 , 1);

	// ------------------------------------------------------------------------
	// Update depth image
	imageStore(depthFilter, pixel, color);
}
