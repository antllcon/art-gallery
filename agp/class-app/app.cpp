#include "app.h"
#include "../config.h"
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <thread>

App::App(int argc, char* argv[])
	: m_state(AppState::Menu)
	, m_stopRender(true)
	, m_showTooltip(true)
{
	InitTooltip();

	std::cout << "-------------------------------------------\n";
	std::cout << "Инструкция по управлению:\n";
	std::cout << "-------------------------------------------\n";
	std::cout << "[O]      - Начать рисование внешнего контура\n";
	std::cout << "[H]      - Начать рисование отверстия (после внешнего контура)\n";
	std::cout << "[Enter]  - Завершить рисование текущей фигуры\n";
	std::cout << "[LMB]    - Поставить точку\n";
	std::cout << "[S]      - Сохранить внешний контур в 'save.txt'\n";
	std::cout << "[Space]  - Запустить алгоритм поиска охранников\n";
	std::cout << "-------------------------------------------\n";
	std::cout << "В режиме просмотра (после Space):\n";
	std::cout << "[R]      - Включить/выключить рендер приложения\n";
	std::cout << "-------------------------------------------\n";

	if (argc > 1)
	{
		std::cout << "\nЗагрузка многоугольника из файла: " << argv[1] << std::endl;
		LoadPolygonFromFile(argv[1]);
	}
}

App::~App() = default;

bool App::IsRender() const
{
	return m_stopRender;
}

void App::InitTooltip()
{
	if (!m_font.loadFromFile(fonts::CASCADILIA_MONO))
	{
		throw std::runtime_error("Не удалось загрузить шрифт: " + fonts::CASCADILIA_MONO);
	}

	m_tooltipText.setFont(m_font);
	m_tooltipText.setString("A program for drawing 2d plans of art galleries\n"
							" for the placement of guards - a demonstration\n"
							"     of the Subir Kumar Ghoshi algorithm\n\n"
							"     Implementation of Glukharev Stepan PS-21");

	m_tooltipText.setCharacterSize(24);
	m_tooltipText.setFillColor(color::WHITE);
	m_tooltipText.setStyle(sf::Text::Bold);

	sf::FloatRect textRect = m_tooltipText.getLocalBounds();
	m_tooltipText.setOrigin(
		textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	m_tooltipText.setPosition(sf::Vector2f(screen::WIDTH / 2.0f, screen::HEIGHT / 2.0f));
}

void App::SavePolygonToFile(const std::string& filename) const
{
	if (!m_polygon.IsOuter())
	{
		std::cout << "Нет внешнего контура для сохранения.\n";
		return;
	}

	std::ofstream outFile(filename);
	if (!outFile)
	{
		std::cerr << "Ошибка: не удалось открыть файл для записи: " << filename
				  << std::endl;
		return;
	}

	const auto& outerPolygon = m_polygon.GetOuter();
	for (const auto& point : outerPolygon)
	{
		outFile << point.first << " " << point.second << "\n";
	}

	outFile.close();
	std::cout << "Многоугольник успешно сохранен в файл: " << filename << std::endl;
}

void App::LoadPolygonFromFile(const std::string& filename)
{
	std::ifstream inFile(filename);
	if (!inFile)
	{
		std::cerr << "Ошибка: не удалось открыть файл для чтения: " << filename
				  << std::endl;
		return;
	}

	Polygon loadedShape;
	double x, y;

	while (inFile >> x >> y)
	{
		loadedShape.emplace_back(x, y);
	}

	inFile.close();

	if (loadedShape.empty())
	{
		std::cerr << "Ошибка: файл пуст или содержит некорректные данные.\n";
		return;
	}

	if (m_polygon.AddOuter(loadedShape))
	{
		m_showTooltip = false;
		m_state = AppState::Play;
		ProtectMuseum();
		std::cout << "Многоугольник успешно загружен.\n";
	}
	else
	{
		m_polygon = PolygonWithHoles();
		std::cerr << "Ошибка: не удалось добавить загруженный многоугольник (возможно, "
					 "он самопересекающийся).\n";
	}
}

void App::ToggleRender()
{
	m_stopRender = !m_stopRender;
}

App::AppState App::GetState() const
{
	return m_state;
}

void App::SetState(AppState newState)
{
	m_state = newState;
}

void App::ProcessEvents(sf::RenderWindow& window)
{
	while (window.pollEvent(m_event))
	{
		CheckCloseApp(window);

		switch (m_state)
		{
		case (AppState::Menu):
			HandleMenuEvents(window);
			break;

		case (AppState::Play):
			HandlePlayEvents();
			break;

		default:
			throw std::runtime_error("Отсутствие состояния приложения");
		}
	}
}

void App::Render(sf::RenderWindow& window)
{
	window.clear(color::BLACK);

	if (m_polygon.IsOuter())
	{
		m_polygon.Draw(window);
	}

	switch (m_state)
	{
	case (AppState::Menu):
		if (m_showTooltip)
		{
			DrawTooltip(window);
		}
		DrawLines(window);
		break;

	case (AppState::Play):
		DrawLight(window, false);
		DrawGuards(window);
		// Отладка разбиения
		// DrawCuts(window, true);
		// DrawCutsOutline(window);
		break;

	default:
		throw std::runtime_error("Отсутствие состояния приложения");
	}

	window.display();
}

void App::CheckCloseApp(sf::RenderWindow& window)
{
	if (m_event.type == sf::Event::Closed)
	{
		window.close();
	}
}

void App::HandleMenuEvents(sf::RenderWindow& window)
{
	// Переписать на состояния (1)
	m_input.ProcessInput();
	if (m_input.IsPressed("outer") && !m_polygon.IsOuter())
	{
		m_showTooltip = false;
		m_drawMode = DrawMode::Outer;
		m_currShape.clear();
	}
	else if (m_input.IsPressed("hole") && m_polygon.IsOuter())
	{
		m_showTooltip = false;
		m_drawMode = DrawMode::Hole;
		m_currShape.clear();
	}
	else if (m_input.IsPressed("play") && !m_currShape.empty())
	{
		if (m_drawMode == DrawMode::Outer)
		{
			if (!m_polygon.AddOuter(m_currShape))
			{
				throw std::runtime_error("Плохо рисуешь внешний контур");
			}
		}
		else if (m_drawMode == DrawMode::Hole)
		{
			if (!m_polygon.AddHole(m_currShape))
			{
				throw std::runtime_error("Плохо рисуешь отверстие");
			}
		}
		m_drawMode = DrawMode::None;
	}
	else if (m_event.type == sf::Event::KeyPressed && m_event.key.code == sf::Keyboard::S
		&& m_polygon.IsOuter())
	{
		SavePolygonToFile("save.txt");
	}
	else if (m_event.type == sf::Event::MouseButtonPressed
		&& m_drawMode != DrawMode::None)
	{
		if (m_event.mouseButton.button == sf::Mouse::Left)
		{
			auto pos = sf::Mouse::getPosition(window);
			sf::Vector2f worldPos = window.mapPixelToCoords(pos);
			m_currShape.emplace_back(
				static_cast<double>(worldPos.x), static_cast<double>(worldPos.y));
		}
	}
	else if (m_drawMode == DrawMode::None && m_event.type == sf::Event::KeyPressed
		&& m_event.key.code == sf::Keyboard::Space && m_polygon.IsOuter())
	{
		m_state = AppState::Play;
		m_polygon.Print();
		ProtectMuseum();
		// Отладка разбиения
		// m_monster = m_polygon.CreateVisualFaces();
	}
}

void App::HandlePlayEvents()
{
	m_input.ProcessInput();
	if (m_event.type == sf::Event::KeyPressed && m_event.key.code == sf::Keyboard::R)
	{
		ToggleRender();
	}

	if (m_event.type == sf::Event::KeyPressed)
	{
		if (m_event.key.code == sf::Keyboard::Right && !m_monster.empty())
		{
			if (m_current_cut_index < m_monster.size() - 1)
			{
				m_current_cut_index++;
				m_cuts = m_monster[m_current_cut_index];
			}
		}
		else if (m_event.key.code == sf::Keyboard::Left && !m_monster.empty())
		{
			if (m_current_cut_index > 0)
			{
				m_current_cut_index--;
				m_cuts = m_monster[m_current_cut_index];
			}
		}
	}
}

void App::ProtectMuseum()
{
	std::cout << std::endl
			  << "Защита музея" << std::endl;

	auto guards = m_polygon.CreatePointsGuards();
	for (const auto& guard : guards)
	{
		m_guards.emplace_back(guard);
	}

	std::cout << "Защита закончена" << std::endl;
	std::cout << "Количество охранников: " << m_guards.size() << std::endl;
	auto n = m_polygon.GetCountPoints();
	auto optimalCountGuards = static_cast<size_t>(std::floor(n / 3));
	std::cout << "Оптимальное решение: " << optimalCountGuards << std::endl;
	std::cout << "Коэффициент аппроксимации: " << log2(n) << std::endl;
}

void App::DrawGuards(sf::RenderWindow& window)
{
	sf::Text text;
	text.setFont(m_font);
	text.setCharacterSize(text::SIZE);
	text.setFillColor(color::BLACK);

	for (size_t i = 0; i < m_guards.size(); ++i)
	{
		const auto& guard = m_guards[i];
		float x = static_cast<float>(guard.first);
		float y = static_cast<float>(guard.second);

		sf::CircleShape guardCircle(constants::RADIUS);
		guardCircle.setFillColor(color::WHITE);
		guardCircle.setOrigin(guardCircle.getRadius(), guardCircle.getRadius());
		guardCircle.setPosition(x, y);
		window.draw(guardCircle);

		text.setString(std::to_string(i));
		sf::FloatRect textBounds = text.getLocalBounds();
		text.setOrigin(textBounds.left + textBounds.width / 2.0f,
			textBounds.top + textBounds.height / 2.0f);
		text.setPosition(x, y);
		window.draw(text);
	}
}

void App::DrawTooltip(sf::RenderWindow& window)
{
	window.draw(m_tooltipText);
}

void App::DrawCuts(sf::RenderWindow& window, bool randomColor)
{
	for (const auto& cut : m_cuts)
	{
		sf::Color rayColor = (randomColor ? GenerateColor() : li::LIGHT);
		sf::ConvexShape poly;
		poly.setPointCount(cut.size());
		for (size_t i = 0; i < cut.size(); ++i)
			poly.setPoint(i,
				sf::Vector2f(
					static_cast<float>(cut[i].first), static_cast<float>(cut[i].second)));

		poly.setFillColor(rayColor);
		poly.setOutlineColor(sf::Color::Transparent);
		poly.setOutlineThickness(0.f);
		window.draw(poly);
	}
}

void App::DrawCutsOutline(sf::RenderWindow& window)
{
	const sf::Color lineColor = sf::Color::White;

	for (const auto& cut : m_cuts)
	{
		if (cut.size() < 2)
			continue;

		sf::VertexArray lines(sf::Lines);

		for (size_t i = 0; i < cut.size(); ++i)
		{
			sf::Vector2f p1(
				static_cast<float>(cut[i].first), static_cast<float>(cut[i].second));
			sf::Vector2f p2(static_cast<float>(cut[(i + 1) % cut.size()].first),
				static_cast<float>(cut[(i + 1) % cut.size()].second));
			lines.append(sf::Vertex(p1, lineColor));
			lines.append(sf::Vertex(p2, lineColor));
		}
		window.draw(lines);
	}
}

void App::DrawLight(sf::RenderWindow& window, bool randomColor)
{
	if (m_guards.empty())
	{
		return;
	}

	m_stopRender = true;

	for (const auto& guard : m_guards)
	{
		sf::Color rayColor = (randomColor ? GenerateColor() : li::LIGHT);
		sf::VertexArray rays(sf::Lines, constants::RAY_COUNT * 2);

		for (int i = 0; i < constants::RAY_COUNT; ++i)
		{
			float angle = i * (2 * M_PI / constants::RAY_COUNT);
			sf::Vector2f direction = { std::cos(angle), std::sin(angle) };

			sf::Vector2f center(
				static_cast<float>(guard.first), static_cast<float>(guard.second));
			rays[2 * i].position = center;

			rays[2 * i + 1].position = center
				+ direction * m_polygon.FindCollision(guard, angle, constants::LENGTH);

			rays[2 * i].color = rayColor;
			rays[2 * i + 1].color = rayColor;
		}

		window.draw(rays);
	}
}

void App::DrawLines(sf::RenderWindow& window) const
{
	if (!m_currShape.empty() && m_drawMode != DrawMode::None)
	{
		sf::VertexArray polyline(sf::LineStrip, m_currShape.size());
		for (size_t i = 0; i < m_currShape.size(); ++i)
		{
			polyline[i].position = sf::Vector2f(static_cast<float>(m_currShape[i].first),
				static_cast<float>(m_currShape[i].second));
			polyline[i].color = sf::Color::Yellow;
		}
		window.draw(polyline);

		if (m_currShape.size() > 1)
		{
			sf::Vertex closingEdge[] = {
				sf::Vertex(sf::Vector2f(static_cast<float>(m_currShape.back().first),
							   static_cast<float>(m_currShape.back().second)),
					sf::Color::Yellow),
				sf::Vertex(sf::Vector2f(static_cast<float>(m_currShape.front().first),
							   static_cast<float>(m_currShape.front().second)),
					sf::Color::Yellow)
			};
			window.draw(closingEdge, 2, sf::Lines);
		}
	}
}

sf::Color App::GenerateColor()
{
	float hue = static_cast<float>(rand() % 360);
	float value = 1.0f;		 // максимальная яркость
	float saturation = 0.8f; // насыщенность

	// HSV to RGB
	float c = value * saturation;
	float x = c * (1 - std::fabs(fmod(hue / 60.0f, 2) - 1));
	float m = value - c;
	float r = 0, g = 0, b = 0;

	if (hue < 60)
	{
		r = c;
		g = x;
		b = 0;
	}
	else if (hue < 120)
	{
		r = x;
		g = c;
		b = 0;
	}
	else if (hue < 180)
	{
		r = 0;
		g = c;
		b = x;
	}
	else if (hue < 240)
	{
		r = 0;
		g = x;
		b = c;
	}
	else if (hue < 300)
	{
		r = x;
		g = 0;
		b = c;
	}
	else
	{
		r = c;
		g = 0;
		b = x;
	}

	return { static_cast<sf::Uint8>((r + m) * 255), static_cast<sf::Uint8>((g + m) * 255),
		static_cast<sf::Uint8>((b + m) * 255), static_cast<sf::Uint8>(constants::ALPHA) };
}
