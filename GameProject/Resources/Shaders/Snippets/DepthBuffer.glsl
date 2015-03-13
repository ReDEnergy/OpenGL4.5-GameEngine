// -----------------------------------------------------------------------------
// Logaritmic zBuffer

vec4 depth() {
	float d = pow(texture(depth_texture, texture_coord).x , 256);
	return vec4(d, d, d, 1.0);
}


// -----------------------------------------------------------------------------
// Reconstruct linear zBuffer from clip space

const float far_plane = 5000.0;
const float near_plane = 0.1;

vec4 linearDepth(sampler2D depthTexture, vec2 coord) {
	float dz = texture(depthTexture, coord).x;
	float z = (2.0 * near_plane) / 
		(far_plane + near_plane - dz * (far_plane - near_plane));
	return vec4(z, z, z, 1.0);
}