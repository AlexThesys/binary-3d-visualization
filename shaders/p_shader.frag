// points fragment shader
# version 440 core

layout (location=0) out vec4 fragColour;

in vec3 pPos;

uniform vec3 colour;
uniform bool monochromatic;

void main()
{
    float col_avg = (pPos.x + pPos.y + pPos.z) * 0.3333f;
    vec4 col_coeff = vec4(col_avg, col_avg, col_avg, 1.0f);
    if (!monochromatic)
      fragColour = vec4(colour, 0.5f) / col_coeff;
    else
      fragColour = vec4(colour, 0.9f) * col_coeff + vec4(0.1f, 0.1f, 0.1f, 0.0f);
}
