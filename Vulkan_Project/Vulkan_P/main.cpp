#include "Framework.h"

int main() {
	system("call shaders/shader.bat");

	auto app = std::make_shared<Framework>();

	try {
		app->run();
	}
	catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}