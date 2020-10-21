#include "Window.h"

const char* APP_TITLE = "Binary 3D visualization";
static constexpr float MOUSE_SENSITIVITY = 0.2f;

Window::Window(GLint w, GLint h) : width(w), height(h) {
	for (size_t i = 0; i < 1024; i++) {
		keys[i] = 0;
	}
}

int Window::initialise(bool wFullScreen) {
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef _APPLE_
	glfwWindowHInt(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // _APPLE_


	if (wFullScreen) {
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* vMode = glfwGetVideoMode(monitor);
		if (vMode != nullptr)
			mainWindow = glfwCreateWindow(vMode->width, vMode->height, APP_TITLE, monitor, nullptr);
	}
	else
	{
		mainWindow = glfwCreateWindow(width, height, APP_TITLE, nullptr, nullptr);
	}
	if (mainWindow == nullptr)
	{
		puts("Failed to create GLFW widnow! Exiting...");
		glfwTerminate();
		return false;
	}

    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
    glfwSetFramebufferSizeCallback(mainWindow, glfw_onFrameBufferSize);

	glfwMakeContextCurrent(mainWindow);

	createCallbacks();

	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(mainWindow, double(bufferWidth) / 2.0, double(bufferHeight) / 2.0);

	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		printf("glew initialization error: %d\n", glewError);
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
        return -1;
	}

	glViewport(0, 0, bufferWidth, bufferHeight);

    //glClearColor(0.1f, 0.2f, 0.2f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glfwSetWindowUserPointer(mainWindow, this);

	lastMousePos = glm::vec2(0.0f, 0.0f);

	return 0;
}

void Window::handleKeys(GLFWwindow *window, int key, int code, int action, int mode) {
	Window *theWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));

	//close the window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		if (theWindow->rotation_speed > 2.0f)
			theWindow->rotation_speed -= 2.0f;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		if (theWindow->rotation_speed < 20.0f)
			theWindow->rotation_speed += 2.0f;
	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
		if (theWindow->data_update_rate > 0x100)
			theWindow->data_update_rate >>= 1;
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
		if (theWindow->data_update_rate < 0x4000)
			theWindow->data_update_rate <<= 1;

	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS)
			theWindow->keys[key] = true;
		else if (action == GLFW_RELEASE)
			theWindow->keys[key] = false;
	}
}

void Window::handleMouse(GLFWwindow *window, double xPos, double yPos) {
	Window *theWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));

	//orbit camera
	//change angles
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == 1)
	{
		theWindow->cam_yaw -= ((GLfloat)xPos - theWindow->lastMousePos.x) * MOUSE_SENSITIVITY;
		theWindow->cam_pitch -= ((GLfloat)yPos - theWindow->lastMousePos.y) * MOUSE_SENSITIVITY;
	}
	//change orbit radius
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == 1)
	{
		float dx = 0.01f * ((GLfloat)xPos - theWindow->lastMousePos.x);
		float dy = 0.01f * ((GLfloat)yPos - theWindow->lastMousePos.y);
		theWindow->cam_radius += dx - dy;
	}
	theWindow->lastMousePos.x = (GLfloat)xPos;
	theWindow->lastMousePos.y = (GLfloat)yPos;
}

void Window::handleScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	Window *theWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));

	theWindow->zoomChange = yoffset *2.0f;
}

void Window::createCallbacks() {
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
	glfwSetScrollCallback(mainWindow, handleScroll);
}

void Window::glfw_onFrameBufferSize(GLFWwindow * window, int width, int height)
{
    Window *theWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));
    theWindow->bufferWidth = width;
    theWindow->bufferHeight = height;
    glViewport(0, 0, width, height);
}

