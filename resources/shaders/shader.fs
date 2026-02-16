#version 460 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
} fs_in;

vec3 lightDirection = vec3(0.0f, 0.5f, 5.0f);

vec3 ambientColor = vec3(0.1, 0.1, 0.1);
vec3 diffuseColor = vec3(0.9, 0.9, 0.9);

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

void main() 
{
    vec3 diffuseSample = texture(diffuseTexture, fs_in.TexCoords).rgb;

    vec3 normalDirection = texture(normalTexture, fs_in.TexCoords).rgb * 2.0 - 1.0;

    float diffuse = max(dot(normalDirection, lightDirection), 0.0);

    vec3 result = (ambientColor + diffuseColor * diffuse) * diffuseSample;
    FragColor = vec4(result, 1.0);
}
