#version 430

layout (binding = 0, rgba32f) uniform image2D colorBuffer;

layout(local_size_x = 16, local_size_y = 16) in;

uniform ivec2 resolution;
uniform ivec2 leftCuePosition;
uniform ivec2 rightCuePosition;

vec4 SoVSplitInput(ivec2 leftCuePos, ivec2 rightCuePos, ivec2 pixel) {
	vec4 overlay = vec4(1.0);
	// Split screen into 4 columns
	vec2 section = resolution / 4;
	for (int i=0; i < 4; i++) {
		float posx = pixel.x - i * section.x;
		if (posx > -2 && posx < 2) {
			overlay *= vec4(0.5);
		}
	}
	
	// Add horizontal line
	if ((pixel.y - (2 * section.y)) > -2 && (pixel.y - (2 * section.y)) < 2) {
		overlay *= vec4(0.5);
	}
	
	// Draw the audioSource in each zone
	float psize = 10;
	
	if ((abs(pixel.x - leftCuePos.x) < psize) &&
		(abs((resolution.y - pixel.y) - leftCuePos.y)) < psize) {
		overlay *= vec4(0.3);
	}

	if ((abs(pixel.x - rightCuePos.x) < psize) &&
		(abs((resolution.y - pixel.y) - rightCuePos.y)) < psize) {
		overlay *= vec4(0.3);
	}
	
	return overlay;
}

void main()
{
	ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
	vec4 overlay = SoVSplitInput(leftCuePosition, rightCuePosition, pixel);
	vec4 color = imageLoad(colorBuffer, pixel);
	imageStore(colorBuffer, pixel, color * overlay);
}
