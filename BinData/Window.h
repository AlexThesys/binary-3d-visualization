#pragma once

#include <cstdio>
#include <random>
#include <memory>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>

struct UpdateData
{
	enum coordinate_system {
		cs_cartesian = 0,
		cs_spherical,
		cs_cylindrical
	};
	enum colour_scheme {
		csc_HYPER = 0,
		csc_MONO,
		csc_LOG2,
		csc_NUM_SCHEMES
	};
	struct RandomEngine {
		std::default_random_engine dre;
		std::uniform_real_distribution<float> urd;
	};
	static constexpr GLuint kinit_block_size = 0x100 << 12; // experimental value
	glm::vec3 data_colour;
	glm::vec2 lastMousePos;
	std::unique_ptr<RandomEngine> rnd_engine;
	GLfloat cam_yaw, cam_pitch, cam_radius;  //mouse coordinates
	GLfloat rotation_speed;
	uint32_t data_update_rate;
	GLint coord_system;
	uint32_t do_update_data;	
	GLuint data_block_size;
	GLint colour_scheme;
	bool draw_unit_cube;
};

class Window {
private:
	GLFWwindow *mainWindow;
	UpdateData* upd_data;
	GLint width, height;
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	static void handleKeys(GLFWwindow *window, int key, int code, int action, int mode);
	static void handleMouse(GLFWwindow *window, double xPos, double yPos);
    static void glfw_onFrameBufferSize(GLFWwindow * window, int width, int height);
public:
	Window(GLint w = 1920, GLint h = 1080, UpdateData* update = nullptr);
	~Window();
	int initialise(UpdateData* update, bool wFullScreen = true);
	GLFWwindow *getWindow() { return mainWindow; }
	bool getShouldClose(){ return glfwWindowShouldClose(mainWindow); }
	void swapBuffers(){ glfwSwapBuffers(mainWindow); }
	GLint getBufferWidth() { return bufferWidth; }
	GLint getBufferHeight() { return bufferHeight; }
	GLfloat get_cam_yaw() const { return upd_data->cam_yaw; }
	GLfloat get_cam_pitch() const { return upd_data->cam_pitch; }
	GLfloat get_cam_radius() const { return upd_data->cam_radius; }
	GLfloat get_rotation_speed() const { return upd_data->rotation_speed; }
	uint32_t get_data_update_rate() const { return upd_data->data_update_rate; }
	uint32_t get_update_data() const { return upd_data->do_update_data; }
	GLint get_coord_system() const { return upd_data->coord_system; }
	bool get_draw_unit_cube() const { return upd_data->draw_unit_cube; }
	GLuint* get_block_size() { return &upd_data->data_block_size; }
	const glm::vec3& get_colour() const { return upd_data->data_colour; }
	GLint get_colour_scheme() const { return upd_data->colour_scheme; }
};

inline Window::~Window() {
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}