#include "class-app/App.h"
#include "config.h"
#include <cstdlib>
#include <iostream>

int main()
{
	try
	{
		sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Pixel Poem");
		App app;

		while (window.isOpen())
		{
			app.ProcessEvents(window);
			app.Render(window);
		}
	}
	catch (const std::exception& errorMessage)
	{
		std::cerr << errorMessage.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}