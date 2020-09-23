#pragma once
#include <GL/glew.h>

static const GLfloat vertices[] = {
    -1.0f, -1.0f, -1.0f,  
     1.0f, -1.0f, -1.0f,  
     1.0f,  1.0f, -1.0f,  
     1.0f,  1.0f, -1.0f,  
    -1.0f,  1.0f, -1.0f,  
    -1.0f, -1.0f, -1.0f,  
         
    -1.0f, -1.0f,  1.0f,  
     1.0f, -1.0f,  1.0f,  
     1.0f,  1.0f,  1.0f,  
     1.0f,  1.0f,  1.0f,  
    -1.0f,  1.0f,  1.0f,  
    -1.0f, -1.0f,  1.0f,  
         
    -1.0f,  1.0f,  1.0f,  
    -1.0f,  1.0f, -1.0f,  
    -1.0f, -1.0f, -1.0f,  
    -1.0f, -1.0f, -1.0f,  
    -1.0f, -1.0f,  1.0f,  
    -1.0f,  1.0f,  1.0f,  
           
     1.0f,  1.0f,  1.0f,  
     1.0f,  1.0f, -1.0f,  
     1.0f, -1.0f, -1.0f,  
     1.0f, -1.0f, -1.0f,  
     1.0f, -1.0f,  1.0f,  
     1.0f,  1.0f,  1.0f,  
         
    -1.0f, -1.0f, -1.0f,  
     1.0f, -1.0f, -1.0f,  
     1.0f, -1.0f,  1.0f,  
     1.0f, -1.0f,  1.0f,  
    -1.0f, -1.0f,  1.0f,  
    -1.0f, -1.0f, -1.0f,  
         
    -1.0f,  1.0f, -1.0f,  
     1.0f,  1.0f, -1.0f,  
     1.0f,  1.0f,  1.0f,  
     1.0f,  1.0f,  1.0f,  
    -1.0f,  1.0f,  1.0f,  
    -1.0f,  1.0f, -1.0f  
};

static constexpr GLint cube_vert_size = sizeof(vertices) / sizeof(GLfloat);
static constexpr GLint cube_num_vert = cube_vert_size / 3;