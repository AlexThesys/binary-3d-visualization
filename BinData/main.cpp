#include <memory>
#include "renderer.h"

int main(int argc, char** argv)
{
	if (argc < 2) {
		puts("Provide the filename!");
		return -1;
	}
	std::unique_ptr<GraphicsData> gdata = std::make_unique<GraphicsData>();
	std::vector<GLfloat> bin_data;
	FileData fdata = {&bin_data, 0, 0};
	initGL(gdata.get(), &fdata, argv[1]);
	renderGL(gdata.get(), &fdata);
	cleanupGL(gdata.get());

	return 0;
}