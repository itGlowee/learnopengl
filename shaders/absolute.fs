#version 330 core
out vec4 color;
in vec2 TexCoords;
uniform vec4 col;

void main() {
    //vec4 sampled = vec4(TexCoords.rg, 0.0f, 1.0f);
    //color = sampled;
    color = col;
}