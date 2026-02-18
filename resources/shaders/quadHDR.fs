#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

const float gamma = 2.2;

uniform float exposure;

uniform sampler2D colorTexture;
uniform sampler2D bloomTexture;

void main() {
    vec3 colorSample = texture(colorTexture, TexCoords).rgb;
    vec3 bloomSample = texture(bloomTexture, TexCoords).rgb;
    vec3 hdrColor = colorSample + bloomSample;
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    vec3 gammaCorrected = pow(mapped, vec3(1.0 / gamma));
    FragColor = vec4(gammaCorrected, 1.0);
}