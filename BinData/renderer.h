#ifndef _RENDERER_
#define _RENDERER_

#include <vector>
#include <GL/glew.h>
#include "Window.h"
#include "Shader.h"

static constexpr unsigned int buf_size = 256u;

struct GraphicsData
{
    Window window;
    Shader pShader, cShader;
    GLuint pVAO, pVBO, cVAO, cVBO;
};

struct FileData
{
    std::vector<GLfloat>* data;
    GLint file_3d_size, block_3d_size;
};

void initGL(GraphicsData* gd, FileData* fd, const char* filename);
void renderGL(GraphicsData* gd, FileData* fd);
void cleanupGL(GraphicsData* gd);

#endif // _RENDERER_
