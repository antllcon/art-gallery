#include "class-app/app.h"
#include "config.h"
#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
	sf::RenderWindow window(
		sf::VideoMode(screen::WIDTH, screen::HEIGHT), "Art Gallery Problem");
	App app(argc, argv);

	while (window.isOpen())
	{
		try
		{
			app.ProcessEvents(window);
			if (app.IsRender())
			{
				app.Render(window);
			}
		}
		catch (const std::exception& errorMessage)
		{
			std::cerr << "Ошибка: " << errorMessage.what() << std::endl;
		}
	}

	return EXIT_SUCCESS;
}