#include "PCH.h"
#include "Core/Application/Application.h"
int main()
{
	srx::Application* app = new srx::Application();
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