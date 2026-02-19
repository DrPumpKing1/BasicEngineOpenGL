#version 460 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

float constant = 1.0;
float linear = 0.7;
float quadratic = 1.8;

vec3 ambientColor = vec3(0.1, 0.1, 0.1);
vec3 diffuseColor = vec3(1.0, 1.0, 1.0);
vec3 specularColor = vec3(2.0, 2.0, 2.0);

float shininess = 32.0;

float heightScale = 0.1;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;

uniform float bloom = 1.0;

void main() 
{
    vec3 viewDirection = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    vec2 texCoords = fs_in.TexCoords;
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;

    vec3 diffuseSample = texture(diffuseTexture, texCoords).rgb;
    vec3 normalSample = texture(normalTexture, texCoords).rgb * 2.0 - 1.0;

    vec3 normalDirection = normalize(normalSample);
    vec3 lightDirection = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);

    float diffuse = max(dot(normalDirection, lightDirection), 0.0);
    float specular = pow(max(dot(normalDirection, halfwayDirection), 0.0), shininess);

    float distance = length(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    vec3 result = ((ambientColor + diffuseColor * diffuse) * diffuseSample + specular * specularColor ) * attenuation;
    FragColor = vec4(result, 1.0);

    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > bloom)
        BrightColor = vec4(result, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}