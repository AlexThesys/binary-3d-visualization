//#include <thread>
#include "Window.h"

const char* APP_TITLE = "Binary 3D visualization";
static constexpr float MOUSE_SENSITIVITY = 0.2f;
static constexpr GLuint kmax_block_size = 0x100 << 14; // experimental value
static constexpr GLuint kmin_block_size = 0x100 << 6; // experimental value

extern void print_help();

static void initUpdateData(UpdateData* upd_data)
{
	upd_data->data_colour = { 0.05f, 0.4f, 0.5f };
	upd_data->lastMousePos = glm::vec2(0.0f, 0.0f);
	upd_data->rnd_engine = std::make_unique<UpdateData::RandomEngine>();
	GLuint seed = GLuint(glfwGetTime());
	upd_data->rnd_engine->dre.seed(seed);
	upd_data->rnd_engine->urd.param(std::uniform_real_distribution<float>::param_type(0.05f, 0.6f));
	upd_data->cam_yaw = 0.0f; 
	upd_data->cam_pitch = 0.0f; 
	upd_data->cam_radius = 0.0f;
	upd_data->rotation_speed = 4.0f;
	upd_data->data_update_rate = 0x1000;
	upd_data->coord_system = UpdateData::cs_cartesian;
	upd_data->do_update_data = 0xffffffff;
	upd_data->data_block_size = UpdateData::kinit_block_size;
	upd_data->colour_scheme = UpdateData::csc_HYPER;
	upd_data->draw_unit_cube = true;
}

Window::Window(GLint w, GLint h, UpdateData* update) : width(w), height(h), upd_data(update) {

}

int Window::initialise(UpdateData* update, bool wFullScreen) {

	upd_data = update;
	initUpdateData(upd_data);

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
			if (theWindow->upd_data->rotation_speed > 0.0f)
				theWindow->upd_data->rotation_speed -= 2.0f;
			break;
		case GLFW_KEY_S:
			if (theWindow->upd_data->rotation_speed < 20.0f)
				theWindow->upd_data->rotation_speed += 2.0f;
			break;
		case GLFW_KEY_Z:
			if (theWindow->upd_data->data_update_rate > 0x100)
				theWindow->upd_data->data_update_rate >>= 1;
			break;
		case GLFW_KEY_X:
			if (theWindow->upd_data->data_update_rate < 0x4000)
				theWindow->upd_data->data_update_rate <<= 1;
			break;
		case GLFW_KEY_D:
			if (theWindow->upd_data->data_block_size > kmin_block_size)
				theWindow->upd_data->data_block_size >>= 1;
			break;
		case GLFW_KEY_F:
			if (theWindow->upd_data->data_block_size < kmax_block_size)
				theWindow->upd_data->data_block_size <<= 1;
			break;
		case GLFW_KEY_Q:
			theWindow->upd_data->coord_system = UpdateData::cs_cartesian;
			break;
		case GLFW_KEY_W:
			theWindow->upd_data->coord_system = UpdateData::cs_spherical;
			break;
		case GLFW_KEY_E:
			theWindow->upd_data->coord_system = UpdateData::cs_cylindrical;
			break;
		case GLFW_KEY_C:
			theWindow->upd_data->draw_unit_cube = !theWindow->upd_data->draw_unit_cube;
			break;
		case GLFW_KEY_SPACE:
			theWindow->upd_data->do_update_data = ~theWindow->upd_data->do_update_data;
			break;
		case GLFW_KEY_T:
			theWindow->upd_data->colour_scheme = ++(theWindow->upd_data->colour_scheme) % UpdateData::csc_NUM_SCHEMES;
			break;
		case GLFW_KEY_H:
			print_help();
			//std::thread(print_help).detach();
			break;
		case GLFW_KEY_R:
			// make this the last one or initialize x,y,z upfront
			const GLfloat x = theWindow->upd_data->rnd_engine->urd(theWindow->upd_data->rnd_engine->dre);
			const GLfloat y = theWindow->upd_data->rnd_engine->urd(theWindow->upd_data->rnd_engine->dre);
			const GLfloat z = theWindow->upd_data->rnd_engine->urd(theWindow->upd_data->rnd_engine->dre);
			theWindow->upd_data->data_colour = { x, y, z };
			break;
		}
	}
}

void Window::handleMouse(GLFWwindow *window, double xPos, double yPos) {
	Window *theWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));

	//orbit camera
	//change angles
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == 1)
	{
		theWindow->upd_data->cam_yaw -= ((GLfloat)xPos - theWindow->upd_data->lastMousePos.x) * MOUSE_SENSITIVITY;
		theWindow->upd_data->cam_pitch -= ((GLfloat)yPos - theWindow->upd_data->lastMousePos.y) * MOUSE_SENSITIVITY;
	}
	//change orbit radius
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == 1)
	{
		float dx = 0.01f * ((GLfloat)xPos - theWindow->upd_data->lastMousePos.x);
		float dy = 0.01f * ((GLfloat)yPos - theWindow->upd_data->lastMousePos.y);
		theWindow->upd_data->cam_radius += dx - dy;
	}
	theWindow->upd_data->lastMousePos.x = (GLfloat)xPos;
	theWindow->upd_data->lastMousePos.y = (GLfloat)yPos;
}

void Window::createCallbacks() {
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

void Window::glfw_onFrameBufferSize(GLFWwindow * window, int width, int height)
{
    Window *theWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));
    theWindow->bufferWidth = width;
    theWindow->bufferHeight = height;
    glViewport(0, 0, width, height);
}

