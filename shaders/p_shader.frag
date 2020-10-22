// points fragment shader
#version 440 core

#define HYPER 0
#define MONO 1
#define LOG2 2

layout (location=0) out vec4 fragColour;

in vec3 pPos;

uniform vec3 colour;
uniform int colour_scheme;

void main()
{
    float col_avg = (pPos.x + pPos.y + pPos.z) * 0.333333f;
    vec4 col_coeff = vec4(col_avg, col_avg, col_avg, 1.0f);
    switch(colour_scheme) {
      case HYPER:
        fragColour = vec4(colour, 0.5f) / col_coeff;
        break;
      case MONO:
        fragColour = vec4(colour, 0.9f) * col_coeff + vec4(0.15f, 0.15f, 0.15f, 0.0f);
        break;
      case LOG2:
        col_avg = log(pPos.x + pPos.y + pPos.z);
        col_coeff = vec4(col_avg, col_avg, col_avg, 1.0f);
        fragColour = vec4(colour, 0.5f) / col_coeff;
        break;
    }
}
