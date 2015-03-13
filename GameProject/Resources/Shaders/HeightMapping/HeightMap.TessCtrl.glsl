#version 410

layout(location = 0) in vec2 v_texture_coord[];
layout(vertices = 3) out;

uniform mat4 View;
uniform mat4 Projection;
uniform uint lod_factor;
uniform vec3 eye_position;

layout(location = 0) out vec2 texture_coord[];

vec4 project(vec4 vertex) {
    vec4 result = Projection * View * vertex;
    result /= result.w;
    return result;
}

bool offScreen(vec4 vertex) {
    if(vertex.z < -0.5)
        return true;

    return any(lessThan(vertex.xz, vec2(-1.3))) || 
			any(greaterThan(vertex.xz, vec2(1.3)));  
}

int level(vec3 v0, vec3 v1) {
     return int(clamp(256/distance(v0, v1), 1, 8));
 }

void main()
{
	 vec4 v0 = project(gl_in[0].gl_Position);
	 vec4 v1 = project(gl_in[1].gl_Position);
	 vec4 v2 = project(gl_in[2].gl_Position);

	if(all(bvec3(offScreen(v0), offScreen(v1), offScreen(v2)))) {
		gl_TessLevelInner[0] = 0;
		gl_TessLevelOuter[0] = 0;
		gl_TessLevelOuter[1] = 0;
		gl_TessLevelOuter[2] = 0;
		return;
	}

	// #define gl_InvocationID ID
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	texture_coord[gl_InvocationID] = v_texture_coord[gl_InvocationID];

	int e0 = level(gl_in[0].gl_Position.xyz, eye_position);
	int e1 = level(gl_in[1].gl_Position.xyz, eye_position);
	int e2 = level(gl_in[2].gl_Position.xyz, eye_position);
	
	// Outer Factor
	gl_TessLevelOuter[0] = e0;
	gl_TessLevelOuter[1] = e1;
	gl_TessLevelOuter[2] = e2;
	
	// Inner Factor
	gl_TessLevelInner[0] = int((e0 + e1 + e2)/3);

}
