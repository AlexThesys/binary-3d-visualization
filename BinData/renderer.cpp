#define GLEW_STATIC
#define _CRT_SECURE_NO_WARNINGS
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "renderer.h"
#include "cubeData.h"

#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049

#define MIN(x,y) (x) < (y) ? (x) : (y)

static constexpr int64_t block_update_speed = 25; // ms
static constexpr size_t kmax_block_size = 0x100 << 12; // experimental value
static constexpr GLfloat rotation_speed = 0.008f; // maybe make configurable
static constexpr GLint data_update_speed = 0x1000; // bytes / block_update_speed

static GLint queryMemoryAvailable()
{
    GLint total_mem_kb = 0;
    glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,
        &total_mem_kb);
    GLint cur_avail_mem_kb = 0;
    glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
        &cur_avail_mem_kb);
    printf("Total graphics memory available kb:\t%d\n", total_mem_kb);
    printf("Graphics memory currently available kb:\t%d\n", cur_avail_mem_kb);

    return cur_avail_mem_kb * 1024;
}

static int load_binary(FileData* fd, GLint mem_size, const char* fname)
{
    FILE* file = fopen(fname, "rb");
    if (!file) {
        puts("Error opening file!");
        return -1;
    }
    fseek(file, 0L, SEEK_END);
    GLint size = ftell(file);
    rewind(file);
    if ((size * sizeof(GLfloat)) >= mem_size) {
        puts("File exceeding maximum size!");
        fclose(file);
        return -1;
    }
    const GLint padding = 3 - (size % 3);
    const GLint padded_size = size + padding;
    fd->data->reserve(padded_size);
    fd->file_3d_size = padded_size / 3;
    fd->block_3d_size = MIN(fd->file_3d_size, kmax_block_size);
 
    const GLint block_read = (sizeof(uint64_t) << 3);
    const GLint reads_left = size & (block_read - 1);
    const GLint reads = size - reads_left;
    for (size_t i = 0lu; i < reads; i += block_read) {
        alignas(16) uint64_t bytes[8];
        fread((void*)(&bytes), 1, block_read, file);
        for (uint16_t k = 0; k < 8; k++) {
            for (uint16_t j = 0; j < (sizeof(uint64_t) << 3); j += 8) {
                GLfloat val = static_cast<GLfloat>((bytes[k] >> j) & 0xff) / 256.0f; // normailze
                val = val * 2.0f - 1.0f; // expand to (-1.0f, 1.0f) range
                fd->data->push_back(val);
            }
        }
    }
    for (uint16_t i = 0; i < reads_left; i++) {
        uint8_t byte;
        fread((void*)(&byte), 1, 1, file);
        GLfloat val = static_cast<GLfloat>(byte) / 256.0f; // normailze
        val = val * 2.0f - 1.0f; // expand to (-1.0f, 1.0f) range
        fd->data->push_back(val);
    }
    for (uint16_t i = 0; i < padding; i++) {
        fd->data->push_back(0.0f);
    }
    return 0;
}

void initGL(GraphicsData* gd, FileData* fd, const char* filename)
{
    // window initialization
    gd->window.initialise();
    const GLint kmax_file_size = queryMemoryAvailable();
    int res = load_binary(fd, kmax_file_size, filename);
    if (res < 0) std::exit(1);

    // create shaders
    gd->cShader.loadShaders("../shaders/c_shader.vert", "../shaders/c_shader.frag");
    gd->cShader.validateProgram();
    gd->pShader.loadShaders("../shaders/p_shader.vert", "../shaders/p_shader.frag");
    gd->pShader.validateProgram();

//---------------------------------------------------------------
    // cube VAO
    glGenVertexArrays(1, &gd->cVAO);
    glGenBuffers(1, &gd->cVBO);
    glBindVertexArray(gd->cVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gd->cVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
                          vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
                                0, (void*)nullptr);
    glEnableVertexAttribArray(0);

    // points VAO
    glGenVertexArrays(1, &gd->pVAO);
    glGenBuffers(1, &gd->pVBO);
    glBindVertexArray(gd->pVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gd->pVBO);
    glBufferData(GL_ARRAY_BUFFER, fd->data->size() * sizeof(GLfloat),
        fd->data->data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                                0, (void*)nullptr);
    glEnableVertexAttribArray(0);

    // unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


}

void renderGL(GraphicsData* gd, FileData* fd)
{
    GLint offset = 0u;

     glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                (float)gd->window.getBufferWidth() / (float)gd->window.getBufferHeight(),
                0.1f, 100.0f);
     glm::mat4 view = glm::mat4(1.0f);
     view = glm::translate(view, glm::vec3(0.0f, 0.0f, -4.0f));
     glm::mat4 VP;

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while(!gd->window.getShouldClose())
    {
        // calculate delta time
        static std::chrono::steady_clock::time_point last_time = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration d = current_time - last_time;
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(d).count();

        // rotate view
        view = glm::rotate(view, (float)glfwGetTime() * glm::radians(rotation_speed), glm::vec3(0.5f, 1.0f, 0.0f));
        VP = projection * view;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // process input
        glfwPollEvents();
    
        // draw cube
        gd->cShader.useProgram();
        gd->cShader.setUniform("VP", VP);

        glBindVertexArray(gd->cVAO);
        glDrawArrays(GL_TRIANGLES, 0, cube_num_vert);

        // draw points
        if (delta >= block_update_speed) {
            last_time = current_time;
            if ((offset + fd->block_3d_size) >= fd->file_3d_size) {
                offset = 0;
            }
            else {
                offset += data_update_speed;
            }
        }
        gd->pShader.useProgram();
        gd->pShader.setUniform("VP", VP);

        glBindVertexArray(gd->pVAO);
        glDrawArrays(GL_POINTS, offset, fd->block_3d_size);
        
        glBindVertexArray(0);

        // swap buffers
        glUseProgram(0);
        gd->window.swapBuffers();
    }
}

void cleanupGL(GraphicsData* gd)
{
    glDeleteVertexArrays(1, &gd->pVAO);
    glDeleteBuffers(1, &gd->pVBO);
    glDeleteVertexArrays(1, &gd->cVAO);
    glDeleteBuffers(1, &gd->cVBO);
    gd->cShader.deleteShader();
    gd->pShader.deleteShader();
}
