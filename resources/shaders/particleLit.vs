#version 430 core
layout (location = 0) in vec2 aTexCoords;
layout (location = 1) in vec3 aPos;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aInstancePos;

#define PI 3.14159265359

out VS_OUT {
    vec2 TexCoords;
    vec2 TexCoordsViewSpace;
    vec3 FragPos;
} vs_out;

layout(std140, binding = 0) uniform Camera
{
    mat4 view;
    mat4 projection;
    vec3 viewPosition;
};

const vec3 worldUp = vec3(0.0, 1.0, 0.0);

mat3 rotateX(float angle);
mat3 rotateY(float angle);
mat3 rotateZ(float angle);

uniform float time;
uniform vec2 offset;
uniform float scale;

void main() 
{
    vs_out.TexCoords = aTexCoords;
    vec3 movedPos = rotateX(time + PI * aInstancePos.x) * rotateY(time * 0.5 + PI * aInstancePos.y) * rotateZ(time * 0.25 + PI * aInstancePos.z) * aInstancePos;
    vec3 Z = normalize(viewPosition - movedPos);
    vec3 X = normalize(cross(worldUp, Z));
    Z = normalize(Z - dot(Z, X) * X);
    vec3 Y = cross(Z, X);
    mat3 viewBasis = mat3(X, Y, Z);
    vs_out.FragPos = viewBasis * aPos + movedPos;
    vs_out.TexCoordsViewSpace = ((transpose(viewBasis) * vs_out.FragPos).xy - offset) * scale;

    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}

mat3 rotateX(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return mat3(1.0, 0.0, 0.0,
                0.0, c, -s,
                0.0, s, c);
}

mat3 rotateY(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return mat3(c, 0.0, s,
                0.0, 1.0, 0.0,
                -s, 0.0, c);
}

mat3 rotateZ(float angle)
{
    float c = cos(angle);
    float s = sin(angle);
    return mat3(c, -s, 0.0,
                s, c, 0.0,
                0.0, 0.0, 1.0);
}