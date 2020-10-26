#ifndef _RENDERER_
#define _RENDERER_

#include <vector>
#include <GL/glew.h>
#include "Window.h"
#include "Shader.h"
#include "Camera.h"

static constexpr unsigned int buf_size = 256u;

struct GraphicsData
{
    Window window;
    OrbitCamera camera;
    Shader pShader, cShader;
    GLuint pVAO, pVBO, cVAO, cVBO;
};

struct FileData
{
    std::vector<GLfloat>* data;
    GLuint* block_3d_size;
    GLuint file_3d_size;
};

void initGL(GraphicsData* gd, FileData* fd, UpdateData* upd, const char* filename, bool full_screen);
void renderGL(GraphicsData* gd, FileData* fd);
void cleanupGL(GraphicsData* gd);

#endif // _RENDERER_
