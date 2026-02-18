#version 460 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D sourceTexture;
  
uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{             
    vec2 texDelta = 1.0 / textureSize(sourceTexture, 0);
    vec3 result = texture(sourceTexture, TexCoords).rgb * weight[0];

    vec2 orientation = horizontal ? vec2(texDelta.x, 0.0) : vec2(0.0, texDelta.y);
    for(int i = 1; i < 5; i++) {
        result += texture(sourceTexture, TexCoords + orientation * float(i)).rgb * weight[i];
        result += texture(sourceTexture, TexCoords - orientation * float(i)).rgb * weight[i];
    }
    FragColor = vec4(result, 1.0);
}