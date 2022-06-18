#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
out vec2 TexCoords;

uniform mat4 projection;
uniform vec2 transform;

void main() {
    gl_Position = projection * vec4(aPos + vec3(transform, 0.0f), 1.0f);
    //gl_Position = projection * vec4(aPos + vec3(transform, 0.0f), 1.0f);
    //gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    TexCoords = aTex;
}