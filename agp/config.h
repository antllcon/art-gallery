#ifndef CONFIG_H
#define CONFIG_H
#include <SFML/Graphics.hpp>

// === Настройки экрана ===
namespace screen
{
constexpr size_t WIDTH = 1920;
constexpr size_t HEIGHT = 1080;
} // namespace screen

// === Цветовые настройки ===
namespace color
{
const sf::Color BLACK = sf::Color(33, 33, 33);
const sf::Color DARK = sf::Color(51, 51, 51);
const sf::Color GRAY = sf::Color(69, 69, 69);
const sf::Color WHITE = sf::Color(255, 255, 255);
const sf::Color TRANSPARENT = sf::Color(0, 0, 0, 0);

const sf::Color PINK = sf::Color(255, 0, 127);
const sf::Color RED = sf::Color(153, 0, 0);
const sf::Color YELLOW = sf::Color(255, 255, 200);
const sf::Color GREEN = sf::Color(102, 153, 102);
const sf::Color BLUE = sf::Color(100, 0, 255);
const sf::Color PURPLE = sf::Color(204, 0, 102);

} // namespace color

// === Существование ребра ===
namespace constants
{
inline constexpr int DELAY = 500;
inline constexpr float LENGTH = 1000.0f;
inline constexpr int RAY_COUNT = 720;
inline constexpr float ALPHA = 0.5f * 255;
inline constexpr size_t THICKNESS = 2;
inline constexpr size_t RADIUS = 10;
} // namespace constants

namespace li
{
const sf::Color LIGHT = sf::Color(247, 234, 72, 0.5f * 255);
}

// === Шрифты ===
namespace fonts
{
const std::string CASCADILIA_MONO = "../fonts/cascadia-mono.ttf";
} // namespace fonts

// === Текст ===
namespace text
{
inline constexpr size_t SIZE = 12;
} // namespace text

#endif