#version 460 core
out vec4 FragColor;

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
vec3 diffuseColor = vec3(0.9, 0.9, 0.9);
vec3 specularColor = vec3(1.0, 1.0, 1.0);

float shininess = 32.0;

float heightScale = 0.1;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D depthTexture;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDirection) 
{
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, max(dot(vec3(0.0, 0.0, 1.0), viewDirection), 0.0));
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

    vec2 approximationDirection = viewDirection.xy * heightScale; 
    vec2 deltaTexCoords = approximationDirection / numLayers;

    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(depthTexture, currentTexCoords).r;
  
    while(currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(depthTexture, currentTexCoords).r;  
        currentLayerDepth += layerDepth;  
    }

    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(depthTexture, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

void main() 
{
    vec3 viewDirection = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    vec2 texCoords = ParallaxMapping(fs_in.TexCoords,  viewDirection);
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
}
