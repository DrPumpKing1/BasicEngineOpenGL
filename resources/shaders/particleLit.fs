#version 430 core
out vec4 FragColor;

in VS_OUT {
    vec2 TexCoords;
    vec2 TexCoordsViewSpace;
    vec3 FragPos;
} fs_in;

uniform sampler2D diffuseTexture;
uniform float radius;

void main()
{
    float distance = length(fs_in.TexCoords - vec2(0.5));

    if (distance > radius)
        discard;
    
    float alpha = 1.0 - (distance / radius);

    if (alpha < 0.5)
        discard;

    vec3 colorSample = texture(diffuseTexture, fs_in.TexCoordsViewSpace).rgb;

    FragColor = vec4(colorSample, alpha);
}