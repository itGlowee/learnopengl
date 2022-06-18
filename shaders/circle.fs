#version 330 core
out vec4 color;
in vec2 TexCoords;
uniform vec4 col;
uniform float thickness = 1.0;

void main() {
    vec4 sampled = vec4(TexCoords, 0.0, 1.0);
    float distance = 1.0 - length(TexCoords);
    //if (distance > 0.0) {
    //    distance = 1.0;
    //}
    //float fade = 0.01;
    float fade = 0.41;
    float alpha = smoothstep(0.0, fade, distance);
    alpha *= smoothstep(thickness + fade, thickness,distance);
    sampled = vec4(col.rgb, col.a * alpha);
    color = sampled;
}

