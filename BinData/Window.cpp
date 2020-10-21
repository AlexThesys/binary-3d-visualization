#include "Window.h"

const char* APP_TITLE = "Binary 3D visualization";
static constexpr float MOUSE_SENSITIVITY = 0.2f;
static constexpr GLuint kmax_block_size = 0x100 << 14; // experimental value
static constexpr GLuint kmin_block_size = 0x100 << 6; // experimental value

Window::Window(GLint w, GLint h) : width(w), height(h) {
	for (size_t i = 0; i < 1024; i++) {
		keys[i] = 0;
	}

	lastMousePos = glm::vec2(0.0f, 0.0f);
	data_colour = { 0.05f, 0.4f, 0.5f };
	rnd_engine = std::make_unique<RandomEngine>();
	GLuint seed = GLuint(glfwGetTime());
	rnd_engine->dre.seed(seed);
	rnd_engine->urd.param(std::uniform_real_distribution<float>::param_type(0.05f, 0.6f));
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

	return 0;
}

void Window::handleKeys(GLFWwindow *window, int key, int code, int action, int mode) {
	Window *theWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_A:
			if (theWindow->rotation_speed > 2.0f)
				theWindow->rotation_speed -= 2.0f;
			break;
		case GLFW_KEY_S:
			if (theWindow->rotation_speed < 20.0f)
				theWindow->rotation_speed += 2.0f;
			break;
		case GLFW_KEY_Z:
			if (theWindow->data_update_rate > 0x100)
				theWindow->data_update_rate >>= 1;
			break;
		case GLFW_KEY_X:
			if (theWindow->data_update_rate < 0x4000)
				theWindow->data_update_rate <<= 1;
			break;
		case GLFW_KEY_D:
			if (theWindow->data_block_size > kmin_block_size)
				theWindow->data_block_size >>= 1;
			break;
		case GLFW_KEY_F:
			if (theWindow->data_block_size < kmax_block_size)
				theWindow->data_block_size <<= 1;
			break;
		case GLFW_KEY_Q:
			theWindow->coord_system = cs_cartesian;
			break;
		case GLFW_KEY_W:
			theWindow->coord_system = cs_spherical;
			break;
		case GLFW_KEY_E:
			theWindow->coord_system = cs_cylindrical;
			break;
		case GLFW_KEY_C:
			theWindow->draw_unit_cube = !theWindow->draw_unit_cube;
			break;
		case GLFW_KEY_SPACE:
			theWindow->update_data = ~theWindow->update_data;
			break;
		case GLFW_KEY_T:
			theWindow->monochromatic = !theWindow->monochromatic;
			break;
		case GLFW_KEY_R:
			const GLfloat x = theWindow->rnd_engine->urd(theWindow->rnd_engine->dre);
			const GLfloat y = theWindow->rnd_engine->urd(theWindow->rnd_engine->dre);
			const GLfloat z = theWindow->rnd_engine->urd(theWindow->rnd_engine->dre);
			theWindow->data_colour = { x, y, z };
			break;

		}
	}

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

