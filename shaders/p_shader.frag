// points fragment shader
# version 440 core

layout (location=0) out vec4 fragColour;

in vec3 pPos;

void main()
{
    float col_avg = (pPos.x + pPos.y + pPos.z) * 0.3333f;
    vec4 col_coeff = vec4(col_avg, col_avg, col_avg, 1.0f);
    fragColour = vec4(0.05f, 0.4f, 0.5f, 0.5f) / col_coeff;
}