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
extern const char* APP_TITLE;

static void showFPS(GLFWwindow* window);

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
    fd->file_3d_size = (GLuint)(padded_size / 3);
    *(fd->block_3d_size) = MIN(fd->file_3d_size, *(fd->block_3d_size));
 
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

void initGL(GraphicsData* gd, FileData* fd, const char* filename, bool full_screen)
{
    // window initialization
    gd->window.initialise(full_screen);
    const GLint kmax_file_size = queryMemoryAvailable();
    fd->block_3d_size = gd->window.get_block_size();
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

     glm::mat4 VP, view, projection;

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while(!gd->window.getShouldClose())
    {
        showFPS(gd->window.getWindow());

        // calculate delta time
        static std::chrono::steady_clock::time_point last_time = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration d = current_time - last_time;
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(d).count();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // process input
        glfwPollEvents();

        gd->camera.setLookAt(glm::vec3(0.0f, 0.0f, 0.0f));
        gd->camera.rotate(gd->window.get_cam_yaw(), gd->window.get_cam_pitch());
        gd->camera.setRadius(gd->window.get_cam_radius());
        view = gd->camera.getViewMatrix();
        // rotate view
        view = glm::rotate(view, glm::radians((float)glfwGetTime() * gd->window.get_rotation_speed()), glm::vec3(0.5f, 1.0f, 0.0f));

        projection = glm::perspective(glm::radians(45.0f),
            (GLfloat)gd->window.getBufferWidth() / (GLfloat)gd->window.getBufferHeight(), 0.1f, 100.0f);
        VP = projection * view;
    
        // draw cube
        if (gd->window.get_draw_unit_cube()) {
            gd->cShader.useProgram();
            gd->cShader.setUniform("VP", VP);

            glBindVertexArray(gd->cVAO);
            glDrawArrays(GL_TRIANGLES, 0, cube_num_vert);
        }
        // draw points
        const GLuint block_size = *(fd->block_3d_size);
        if (delta >= block_update_speed) {
            last_time = current_time;
            if ((offset + block_size) >= fd->file_3d_size) {
                offset = 0;
            }
            else {
                offset += gd->window.get_data_update_rate() & gd->window.get_update_data();
            }
        }
        gd->pShader.useProgram();
        gd->pShader.setUniform("VP", VP);
        gd->pShader.setUniform("coord_system", gd->window.get_coord_system());
        gd->pShader.setUniform("colour", gd->window.get_colour());
        gd->pShader.setUniform("monochromatic", gd->window.get_monochromatic());

        glBindVertexArray(gd->pVAO);
        glDrawArrays(GL_POINTS, offset, block_size);
        
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

static void showFPS(GLFWwindow* window)
{
    static double previousSeconds = 0.0;
    static int frameCount = 0;
    double elapseSeconds;
    double currentSeconds = glfwGetTime();

    elapseSeconds = currentSeconds - previousSeconds;

    //limit texxt update 4 times per second
    if (elapseSeconds > 0.25)
    {
        previousSeconds = currentSeconds;
        double fps = (double)frameCount / elapseSeconds;
        double msPerFrame = 1000.0 / fps;
        /*
        std::ostringstream outs;
        outs.precision(3);
        outs << std::fixed << APP_TITLE << "  "
            << "FPS: " << fps << "  "
            << "Frame Time: " << msPerFrame << "(ms)";
        glfwSetWindowTitle(window, outs.str().c_str());
        */
        char outs[128];
        sprintf(outs, "%s\tFPS: %.3f\tFrame Time: %.3f (ms)\n", APP_TITLE, fps, msPerFrame);
        glfwSetWindowTitle(window, outs);

        frameCount = 0;
    }
    ++frameCount;
}