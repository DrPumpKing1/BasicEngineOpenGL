#version 460 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

struct Material {
    sampler2D diffuse1;
    sampler2D specular1;
    sampler2D normal1;
};

float constant = 1.0;
float linear = 0.22;
float quadratic = 0.20;

vec3 ambientColor = vec3(0.1, 0.1, 0.1);
vec3 diffuseColor = vec3(1.0, 1.0, 1.0);
vec3 specularColor = vec3(2.0, 2.0, 2.0);

float shininess = 32.0;

float heightScale = 0.1;

uniform Material material;

void main() 
{
    FragColor = vec4(1.0);

    vec3 viewDirection = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    vec2 texCoords = fs_in.TexCoords;

    vec3 diffuseSample = texture(material.diffuse1, texCoords).rgb;
    vec3 specularSample = texture(material.specular1, texCoords).rgb;
    vec3 normalSample = texture(material.normal1, texCoords).rgb * 2.0 - 1.0;

    vec3 normalDirection = normalize(normalSample);
    vec3 lightDirection = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);

    float diffuse = max(dot(normalDirection, lightDirection), 0.0);
    float specular = pow(max(dot(normalDirection, halfwayDirection), 0.0), shininess);

    float distance = length(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));

    vec3 result = ((ambientColor + diffuseColor * diffuse) * diffuseSample + specular * specularSample * specularColor ) * attenuation;
    FragColor = vec4(result, 1.0);
}