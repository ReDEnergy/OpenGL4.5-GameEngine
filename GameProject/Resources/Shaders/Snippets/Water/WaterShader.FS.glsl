precision mediump float;


uniform sampler2D s_texture2D_0;
uniform sampler2D s_texture2D_1;
uniform samplerCube s_textureCube_0;
uniform vec3 u_eye;
uniform float u_time;

varying vec3 v_posW;
varying vec3 v_normW;
varying vec2 v_uv;

void main()
{
/*
	vec3 distort	= 2.0*normalize(texture2D(s_texture2D_1, v_uv + u_time).xyz) -1.0;
	vec3 norm		= normalize(v_normW);
	
	vec3 toFrag		= normalize(v_posW - u_eye);
	vec3 dir = normalize(reflect(toFrag, norm ) + distort*0.5 );
	vec3 culoareSky = textureCube(s_textureCube_0,-dir ).xyz;
	
	vec3 culoare	= texture2D(s_texture2D_0, v_uv*5.0 ).xyz;
	
	float factor = abs(dot(-toFrag,norm));
	factor = factor*factor;
	
	gl_FragColor = vec4(culoare*factor + culoareSky*(1.0-factor),1.0);	
*/

	vec3 distort	= 2.0*normalize(texture2D(s_texture2D_1, v_uv + u_time * 0.2).xyz) -1.0;
	vec3 norm		= normalize(v_normW);
	
	vec3 toEye		= normalize(v_posW - u_eye);
	vec3 dir = normalize(reflect(toEye, norm ) + distort*0.5 );
	vec3 culoareSky = textureCube(s_textureCube_0,-dir ).xyz;
	
	float dep		= length(cross(toEye,normalize(toEye+dir)));
	vec3 culoare	= texture2D(s_texture2D_0, v_uv + vec2(toEye.x,-toEye.z)*dep/sqrt(1.0-dep*dep)*v_uv.x*1.0).xyz ;
	float coef		= clamp(v_uv.x*3.0,0.0,1.0);
	culoare	= culoare* (1.0 - coef) + coef*vec3(0.0,0.0,1.0);
	float factor = abs(dot(toEye,norm));
	factor = factor*factor;
	
	gl_FragColor = vec4(culoare*factor + culoareSky*(1.0-factor),1.0);
}