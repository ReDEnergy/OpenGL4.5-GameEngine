#version 310 es
precision highp float;

layout(location = 0) out vec4 fragColor;

uniform vec4 debug_color;

void main() {
	fragColor = debug_color;
}