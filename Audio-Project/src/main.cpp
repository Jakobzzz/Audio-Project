/*
	Entrypoint of the application
*/

#include <graphics/Application.hpp>

int main(int, char**)
{
	px::Application app;
	app.Initialize();
	app.Run();
	app.ShutDown();

	return EXIT_SUCCESS;
}
