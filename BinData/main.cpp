#include <memory>
#include "renderer.h"

enum coordinate_system {
	cs_cartesian = 0,
	cs_spherical,
	cs_cylindrical
};

int main(int argc, char** argv)
{
	if (argc < 2) {
		puts("Provide the filename!");
		puts("Also if you want to use spherical coordinates instead of cartesian add '-s'!");
		puts("Or if you want to use cylindical coordinates add '-c'!");
		return -1;
	}
	GLint coord_system = cs_cartesian; 
	if (argc > 2) {
		if (argv[2][1] == 's') coord_system = cs_spherical;
		if (argv[2][1] == 'c') coord_system = cs_cylindrical;
	}
	std::unique_ptr<GraphicsData> gdata = std::make_unique<GraphicsData>();
	std::vector<GLfloat> bin_data;
	FileData fdata = {&bin_data, 0, 0};
	initGL(gdata.get(), &fdata, argv[1], coord_system);
	renderGL(gdata.get(), &fdata);
	cleanupGL(gdata.get());

	return 0;
}