// points vertex shader
#version 440 core

#define PI 3.1415926538f
#define TWO_PI 6.2831853076
#define CARTESIAN 0
#define SPHERICAL 1
#define CYLINDRICAL 2

layout (location=0) in vec3 pos;

uniform mat4 VP;
uniform int coord_system;

out vec3 pPos;

void main()
{

  vec3 position = pos;
  if (coord_system > 0) {
    const float radius = position.x;
    switch(coord_system)
    {
      case SPHERICAL :
        const float inclination = position.y * PI;
        const float azimuth = position.z * TWO_PI;
        const float rad_sin_incl = radius * sin(inclination);
        position.x = rad_sin_incl * cos(azimuth);
        position.y = rad_sin_incl * sin(azimuth);
        position.z = radius * cos(inclination);
      break;
      case CYLINDRICAL :
        const float angle = position.y * TWO_PI;
        position.x = radius * cos(angle);
        position.y = radius * sin(angle);
        // position.z stays the same
      break;
    }
}
  // else use CARTESIAN
  gl_Position = VP * vec4(position, 1.0f);
  pPos = abs(position);
}
