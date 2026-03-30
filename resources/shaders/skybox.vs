#version 460 core
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

layout(std140, binding = 0) uniform Camera
{
    mat4 view;
    mat4 projection;
    vec3 viewPosition;
};

void main() {
    texCoords = aPos;
    vec4 pos = projection * mat4(mat3(view)) * vec4(aPos, 1.0f);
    gl_Position = pos.xyww;
}