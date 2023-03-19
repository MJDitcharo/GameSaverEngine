
#include "gs_app.h"

// std
#include <cstdlib>
#include <iostream>
#include <fstream>

int main()
{
	md::GSApp app{};



	try
	{
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}