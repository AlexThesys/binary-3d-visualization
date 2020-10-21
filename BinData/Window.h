#pragma once

#include <cstdio>
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
	GLFWwindow *mainWindow;
	GLint width, height;
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	static void handleKeys(GLFWwindow *window, int key, int code, int action, int mode);
	bool keys[1024];  //ASCI keys
	glm::vec2 lastMousePos;
	GLfloat cam_yaw, cam_pitch, cam_radius, zoomChange;  //mouse coordinates
	GLfloat rotation_speed = 4.0f;
	GLint data_update_rate = 0x1000;
	GLint coord_system = cs_cartesian;
	bool draw_unit_cube = true;
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
	GLint get_data_update_rate() const { return data_update_rate; }
	GLint get_coord_system() const { return coord_system; }
	bool get_draw_unit_cube() const { return draw_unit_cube; }
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
