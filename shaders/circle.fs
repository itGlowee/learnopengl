#version 330 core
out vec4 color;
in vec2 TexCoords;

void main() {
    //vec4 sampled = vec4(TexCoords.x, 1.0, 1.0, 0.0);
    vec4 sampled = vec4(1.0, 1.0, 1.0, 0.0);
    color = sampled;
}

