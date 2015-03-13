float FogFactor(vec3 w_pos) {
    float fogStart = 1.0;
    float fogRange = 30.0;

    float dist = distance(w_pos, eye_position);

    float fogFactor = clamp( (dist - fogStart) / fogRange, 0.0, 1.0 );
    return fogFactor;
}