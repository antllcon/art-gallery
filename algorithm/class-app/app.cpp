#include "app.h"
#include "../class-graph/graph.h"
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

void App::CheckCloseApp(sf::RenderWindow& window, const sf::Event& event)
{
	if (event.type == sf::Event::Closed)
	{
		// TODO: Подумать про деструкторы
		window.close();
	}
}

void App::ProcessEvents(sf::RenderWindow& window, Graph& grpah)
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		CheckCloseApp(window, event);

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
}

void App::Render(sf::RenderWindow& window, Graph& graph)
{
	switch (state)
	{
	case (AppState::Menu):
		window.clear(color::BLACK);
		graph.Draw(window);
		break;

	case (AppState::Play):
		window.clear(color::BLACK);
		break;

	default:
		throw std::runtime_error("Отсутствие состояния приложения");
		break;
	}

	window.display();
}

void App::HandleMenuEvents(sf::RenderWindow& window)
{
	inputHandler.ProcessInput();
	// придумать меню
}

void App::HandlePlayEvents()
{
	// написать структуру для цикла алгоритма
}