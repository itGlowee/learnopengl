#version 330 core
out vec4 color;
in vec2 TexCoords;
uniform vec4 col;
uniform float thickness = 1.0;

void main() {
    vec2 uv = TexCoords * 2.0 - 1.0;
    vec4 sampled = vec4(uv, 0.0, 1.0);
    float distance = 1.0 - length(uv);
    //if (distance > 0.0) {
    //    distance = 1.0;
    //}
    //float fade = 0.01;
    float fade = 0.01;
    float alpha = smoothstep(0.0, fade, distance);
    alpha *= smoothstep(thickness + fade, thickness,distance);
    sampled = vec4(col.rgb, col.a * alpha);
    color = sampled;
}

