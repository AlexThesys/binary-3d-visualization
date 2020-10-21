#pragma once

#include <cstdio>
#include <random>
#include <memory>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>

class Window {
private:
	enum coordinate_system {
		cs_cartesian = 0,
		cs_spherical,
		cs_cylindrical
	};
	struct RandomEngine {
		std::default_random_engine dre;
		std::uniform_real_distribution<float> urd;
	};
	glm::vec3 data_colour;
	GLFWwindow *mainWindow;
	std::unique_ptr<RandomEngine> rnd_engine;
	GLint width, height;
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	static void handleKeys(GLFWwindow *window, int key, int code, int action, int mode);
	bool keys[1024];  //ASCI keys
	glm::vec2 lastMousePos;
	GLfloat cam_yaw, cam_pitch, cam_radius, zoomChange;  //mouse coordinates
	GLfloat rotation_speed = 4.0f;
	uint32_t data_update_rate = 0x1000;
	GLint coord_system = cs_cartesian;
	uint32_t update_data = 0xffffffff;
	static constexpr GLuint kinit_block_size = 0x100 << 12; // experimental value
	GLuint data_block_size = kinit_block_size;
	bool draw_unit_cube = true;
	bool monochromatic = false;
	static void handleMouse(GLFWwindow *window, double xPos, double yPos);
	static void handleScroll(GLFWwindow* window, double xoffset, double yoffset);
    static void glfw_onFrameBufferSize(GLFWwindow * window, int width, int height);
public:
	Window(GLint w = 1920, GLint h = 1080);
	~Window();
	int initialise(bool wFullScreen = true);
	GLFWwindow *getWindow() { return mainWindow; }
	bool getShouldClose(){ return glfwWindowShouldClose(mainWindow); }
	void swapBuffers(){ glfwSwapBuffers(mainWindow); }
	GLint getBufferWidth() { return bufferWidth; }
	GLint getBufferHeight() { return bufferHeight; }
	bool *getKeys() { return keys; }
	GLfloat get_cam_yaw() const { return cam_yaw; }
	GLfloat get_cam_pitch() const { return cam_pitch; }
	GLfloat get_cam_radius() const { return cam_radius; }
	GLfloat getZoomChange();
	GLfloat get_rotation_speed() const { return rotation_speed; }
	uint32_t get_data_update_rate() const { return data_update_rate; }
	uint32_t get_update_data() const { return update_data; }
	GLint get_coord_system() const { return coord_system; }
	bool get_draw_unit_cube() const { return draw_unit_cube; }
	GLuint* get_block_size() { return &data_block_size; }
	const glm::vec3& get_colour() const { return data_colour; }
	bool get_monochromatic() const { return monochromatic; }
};

inline Window::~Window() {
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}


inline GLfloat Window::getZoomChange() {
	GLfloat theChange = zoomChange;
	zoomChange = 0.0f;
	return theChange;
}
