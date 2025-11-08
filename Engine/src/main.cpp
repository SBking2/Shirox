#include "Core/Application/Application.h"

#include <iostream>
#include <exception>
#include <vector>
int main()
{
	ev::Application* app = new ev::Application();
	app->Init();

	try
	{
		app->Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		app->Clear();
		delete app;
		return EXIT_FAILURE;
	}

	app->Clear();
	delete app;
	return EXIT_SUCCESS;
}