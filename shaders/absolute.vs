#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 transform;

void main() {
    gl_Position = projection * vec4(aPos + 0.1, 1.0);
}