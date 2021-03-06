const char* p_vertex_shader_code = "#version 440 core\n"
"#define PI 3.1415926538f\n"
"#define TWO_PI 6.2831853076\n"
"#define CARTESIAN 0\n"
"#define SPHERICAL 1\n"
"#define CYLINDRICAL 2\n"
"layout(location = 0) in vec3 pos;\n"
"uniform mat4 VP;\n"
"uniform int coord_system;\n"
"out vec3 pPos;\n"
"void main()\n"
"{\n"
"    vec3 position = pos;\n"
"    if (coord_system > 0) {\n"
"        const float radius = position.x;\n"
"        switch (coord_system)\n"
"        {\n"
"        case SPHERICAL:\n"
"            const float inclination = position.y * PI;\n"
"            const float azimuth = position.z * TWO_PI;\n"
"            const float rad_sin_incl = radius * sin(inclination);\n"
"            position.x = rad_sin_incl * cos(azimuth);\n"
"            position.y = rad_sin_incl * sin(azimuth);\n"
"            position.z = radius * cos(inclination);\n"
"            break;\n"
"        case CYLINDRICAL:\n"
"            const float angle = position.y * TWO_PI;\n"
"            position.x = radius * cos(angle);\n"
"            position.y = radius * sin(angle);\n"
"            break;\n"
"        }\n"
"    }\n"
"    gl_Position = VP * vec4(position, 1.0f);\n"
"    pPos = abs(position);\n"
"}\n"
;

const char* p_fragment_shader_code = "#version 440 core\n"
"#define HYPER 0\n"
"#define MONO 1\n"
"#define LOG2 2\n"
"layout(location = 0) out vec4 fragColour;\n"
"in vec3 pPos;\n"
"uniform vec3 colour;\n"
"uniform int colour_scheme;\n"
"void main()\n"
"{\n"
"    float col_avg = (pPos.x + pPos.y + pPos.z) * 0.333333f;\n"
"    vec4 col_coeff = vec4(col_avg, col_avg, col_avg, 1.0f);\n"
"    switch (colour_scheme) {\n"
"    case HYPER:\n"
"        fragColour = vec4(colour, 0.5f) / col_coeff;\n"
"        break;\n"
"    case MONO:\n"
"        fragColour = vec4(colour, 0.9f) * col_coeff + vec4(0.15f, 0.15f, 0.15f, 0.0f);\n"
"        break;\n"
"    case LOG2:\n"
"        col_avg = log(pPos.x + pPos.y + pPos.z);\n"
"        col_coeff = vec4(col_avg, col_avg, col_avg, 1.0f);\n"
"        fragColour = vec4(colour, 0.5f) / col_coeff;\n"
"        break;\n"
"}\n"
"}\n"
;

const char* c_vertex_shader_code = "#version 440 core\n"
"layout(location = 0) in vec3 pos;\n"
"uniform mat4 VP;\n"
"void main()\n"
"{\n"
"    gl_Position = VP * vec4(pos, 1.0f);\n"
"}\n"
;

const char* c_fragment_shader_code = "#version 440 core\n"
"layout(location = 0) out vec4 fragColour;\n"
"void main()\n"
"{\n"
"    fragColour = vec4(0.0f, 0.2f, 0.35f, 0.5f);\n"
"}\n"
;