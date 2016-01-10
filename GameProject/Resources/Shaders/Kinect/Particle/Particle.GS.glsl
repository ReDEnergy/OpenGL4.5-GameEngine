#version 430

layout(points) in;
layout(triangle_strip, max_vertices = 14) out;

struct particle {
	vec4 position;
	vec4 stability;
};

layout(binding=0) buffer particles {
		particle data[];
};
layout(location = 0) in vec4 info[];

uniform mat4 View;
uniform mat4 Projection;
uniform vec3 eye_position;

layout(location = 0) out vec2 text_coord;
layout(location = 1) out vec3 world_position;
layout(location = 2) out vec3 world_normal;
layout(location = 3) out vec3 v_color;

vec3 wup = vec3(0, 1.0, 0);
vec3 forward = normalize(eye_position - gl_in[0].gl_Position.xyz);
vec3 right = normalize(cross(forward, wup));
vec3 up = normalize(cross(forward, right));

vec4 GetPosition(vec3 offset) {
	vec3 pos = offset + gl_in[0].gl_Position.xyz;
//	vec3 pos = right * offset.x + up * offset.y + gl_in[0].gl_Position.xyz;
	return Projection * View * vec4(pos, 1.0);
}

void EmitPoint(vec3 offset) {
	gl_Position = GetPosition(offset);
	EmitVertex();
}

void main() {
	
	int index = int(info[0].x);
	vec4 pos = data[index].position;

	// Discard Kinect errors 
	// if (abs(pos.z) < 1.0) return;
	// if (pos.x == 0 || pos.y == 0 || pos.z == 0) return;
	
	// v_color = pos.a > 0.5 ? vec3(pos.z / (128)) : vec3(1.0, 0, 0);
	v_color = data[index].stability.rgb;
	
	float ds = 0.1;
	// Crate a box using triangle_strip
	EmitPoint(vec3(-ds,  ds, -ds));			// Point 4
	EmitPoint(vec3( ds,  ds, -ds));         // Point 3
	EmitPoint(vec3(-ds,  ds,  ds));         // Point 7
	EmitPoint(vec3( ds,  ds,  ds));         // Point 8
	EmitPoint(vec3( ds, -ds,  ds));         // Point 5
	EmitPoint(vec3( ds,  ds, -ds));         // Point 3
	EmitPoint(vec3( ds, -ds, -ds));         // Point 1
	EmitPoint(vec3(-ds,  ds, -ds));         // Point 4
	EmitPoint(vec3(-ds, -ds, -ds));         // Point 2
	EmitPoint(vec3(-ds,  ds,  ds));         // Point 7
	EmitPoint(vec3(-ds, -ds,  ds));         // Point 6
	EmitPoint(vec3( ds, -ds,  ds));         // Point 5
	EmitPoint(vec3(-ds, -ds, -ds));         // Point 2
	EmitPoint(vec3( ds, -ds, -ds));         // Point 1
	EndPrimitive();
}
