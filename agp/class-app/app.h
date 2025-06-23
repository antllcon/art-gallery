#ifndef APP_H
#define APP_H

#include "../class-graph/polygon-with-holes.h"
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

	enum class DrawMode
	{
		Outer,
		Hole,
		None
	};

public:
	App(int argc, char* argv[]);
	~App();

	bool IsRender() const;
	void ToggleRender();
	AppState GetState() const;
	void SetState(AppState newState);
	void ProcessEvents(sf::RenderWindow& window);
	void Render(sf::RenderWindow& window);

private:
	void CheckCloseApp(sf::RenderWindow& window);
	void HandleMenuEvents(sf::RenderWindow& window);
	void HandlePlayEvents();
	void ProtectMuseum();

	// Отрисовка
	void DrawLines(sf::RenderWindow& window) const;
	void DrawLight(sf::RenderWindow& window, bool randomColor);
	void DrawGuards(sf::RenderWindow& window);
	void DrawCuts(sf::RenderWindow& window, bool randomColor);
	void DrawCutsOutline(sf::RenderWindow& window);
	void DrawTooltip(sf::RenderWindow& window);

	// Вспомогательные методы
	sf::Color GenerateColor();
	void InitTooltip();

	// Методы для работы с файлами
	void SavePolygonToFile(const std::string& filename) const;
	void LoadPolygonFromFile(const std::string& filename);

private:
	AppState m_state = AppState::Menu;
	DrawMode m_drawMode = DrawMode::None;
	sf::Event m_event;
	Input m_input;
	bool m_stopRender;
	bool m_showTooltip;

	sf::Font m_font;
	sf::Text m_tooltipText;

	PolygonWithHoles m_polygon;
	Polygon m_currShape;
	Points m_guards;
	Polygons m_cuts;
	std::vector<Polygons> m_monster;

	size_t m_current_cut_index = 0;
};

#endif