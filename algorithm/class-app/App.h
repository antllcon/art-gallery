#ifndef APP_H
#define APP_H

#include "../class-input/Input.h"
#include <SFML/Graphics.hpp>

/// @brief Класс приложения
class App
{

private:
	/// @brief Состояния приложения
	enum class AppState
	{
		Menu,
		Play
	};

	sf::Event event;
	AppState state;
	Input inputHandler;

	void CheckCloseApp(sf::RenderWindow& window);
	void HandleMenuEvents(sf::RenderWindow& window);
	void HandlePlayEvents();

public:
	App();
	~App();

	AppState GetState() const;
	void SetState(AppState newState);
	void ProcessEvents(sf::RenderWindow& window);
	void Render(sf::RenderWindow& window);
};

#endif