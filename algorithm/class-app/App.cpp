#include "App.h"
#include "../config.h"

App::App()
	: state(AppState::Menu)
{
}

App::~App() = default;

void ProcessEvents(sf::RenderWindow& window)
{
}

App::AppState App::GetState() const
{
	return state;
}

void App::SetState(AppState newState)
{
	state = newState;
}

void App::CheckCloseApp(sf::RenderWindow& window)
{
	if (event.type == sf::Event::Closed)
	{
		// TODO: Подумать про деструкторы
		window.close();
	}
}

void App::ProcessEvents(sf::RenderWindow& window)
{
	CheckCloseApp(window);

	switch (state)
	{
	case (AppState::Menu):
		HandleMenuEvents(window);
		break;

	case (AppState::Play):
		HandlePlayEvents();
		break;

	default:
		throw std::runtime_error("Отсутствие состояния приложения");
		break;
	}
}

void App::Render(sf::RenderWindow& window)
{
	switch (state)
	{
	case (AppState::Menu):
		window.clear(COLOR_BLACK);
		break;

	case (AppState::Play):
		window.clear(COLOR_BLACK);
		break;

	default:
		throw std::runtime_error("Отсутствие состояния приложения");
		break;
	}

	window.display();
}

void App::HandleMenuEvents(sf::RenderWindow& window)
{
}

void App::HandlePlayEvents()
{
}