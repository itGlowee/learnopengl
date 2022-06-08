#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform vec2 transform;

void main() {
    gl_Position = projection * vec4(aPos + vec3(transform, 0.0f), 1.0);
}