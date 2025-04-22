#ifndef APP_H
#define APP_H

#include "../class-graph/graph.h"
#include "../class-input/input.h"
#include <SFML/Graphics.hpp>
#include <memory>

/// @brief Класс приложения
class App
{
	/// @brief Состояния приложения
	enum class AppState
	{
		Menu,
		Play
	};

public:
	App();
	~App();

	AppState GetState() const;
	void SetState(AppState newState);
	void ProcessEvents(sf::RenderWindow& window, Graph& graph);
	void Render(sf::RenderWindow& window, Graph& graph);

private:
	AppState state;
	sf::Event event;
	Input inputHandler;

	std::unique_ptr<Graph> graph;

	void CheckCloseApp(sf::RenderWindow& window, const sf::Event& event);
	void HandleMenuEvents(sf::RenderWindow& window);
	void HandlePlayEvents();
};

#endif