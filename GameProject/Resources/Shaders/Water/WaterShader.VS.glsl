uniform mat4 u_WVP;
uniform mat4 u_world; 

attribute vec3 a_posL;
attribute vec3 a_normL;
attribute vec2 a_uv;

varying vec3 v_posW;
varying vec3 v_normW;
varying vec2 v_uv;

void main()
{ 	
	v_posW		= (u_world*vec4(a_posL,1.0)).xyz;
	v_normW		= (u_world*vec4(a_normL,0.0)).xyz;
	v_uv		= a_uv;
	
	gl_Position = u_WVP * vec4(a_posL, 1.0);
}
   