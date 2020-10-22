#include <memory>
#include "renderer.h"

void print_help()
{
	puts("------------------------------------");
	puts("Press 'h' to print this help...");
	puts("Press ESC to quit...");
	puts("Press 'a' or 's' keys to change rotation speed...");
	puts("Press 'z' or 'x' keys to change data update rate...");
	puts("Press 'd' or 'f' to change data block size...");
	puts("Press 'q', 'w' or 'e' to select cartesian, spherical or cylindrical coordinate system...");
	puts("Press 'c' to draw unit cube wireframe...");
	puts("Press 'r' to randomize colours...");
	puts("Press 't' to toggle colour scheme...");
	puts("Press SPACE to stop updating data blocks...");
	puts("Press LMB + drag to rotate camera...");
	puts("Press RMB + drag to zoom in/out...");
	puts("------------------------------------");
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		puts("Provide the path to the file!");
		return -1;
	}
	print_help();
	std::unique_ptr<GraphicsData> gdata = std::make_unique<GraphicsData>();
	std::vector<GLfloat> bin_data;
	FileData fdata = {&bin_data, 0, 0};
	initGL(gdata.get(), &fdata, argv[1], true);
	renderGL(gdata.get(), &fdata);
	cleanupGL(gdata.get());

	return 0;
}