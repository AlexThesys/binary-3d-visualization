#include <memory>
#include "renderer.h"

int main(int argc, char** argv)
{
	puts("Press 'a' or 's' keys to change rotation speed...");
	puts("Press 'z' or 'x' keys to change data update rate...");
	puts("Press 'd' or 'f' to change data block size...");
	puts("Press 'q', 'w' or 'e' to select cartesian, spherical or cylindrical coordinate system...");
	puts("Press 'c' to draw unit cube wireframe...");
	puts("Press SPACE to stop updating data blocks...");
	puts("Press LMB + drag to rotate camera...");
	puts("Press RMB + drag to zoom in/out...");
	puts("------------------------------------");
	
	if (argc < 2) {
		puts("Provide the filename!");
		return -1;
	}
	std::unique_ptr<GraphicsData> gdata = std::make_unique<GraphicsData>();
	std::vector<GLfloat> bin_data;
	FileData fdata = {&bin_data, 0, 0};
	initGL(gdata.get(), &fdata, argv[1], true);
	renderGL(gdata.get(), &fdata);
	cleanupGL(gdata.get());

	return 0;
}