#ifndef POLYGON_H
#define POLYGON_H

#include "../config.h"
#include "../types.hpp"
#include <SFML/Graphics.hpp>

class PolygonWithHoles
{
public:
	PolygonWithHoles() = default;
	PolygonWithHoles(const Polygon& outher, const Polygons& holes);

	// Геттеры и свойства
	const Polygon& GetOuter() const;
	const Polygons& GetHoles() const;
	size_t GetCountPoints() const;
	Points GetPoints() const;
	Edges GetEdges() const;

	// Проверки
	bool IsOuter() const;
	bool IsHoles() const;

	// Модификация многоугольника
	bool AddOuter(const Polygon& shape);
	bool AddHole(const Polygon& hole);

	// SFML отрисовка
	void Print() const;
	void Draw(sf::RenderWindow& window) const;
	float FindCollision(const Point& guard, float angle, float length) const;

	Polygons GetViewFaces();
	HistoryPoly CreateVisualFaces();

	// Главная функция
	Points CreatePointsGuards();

private:
	void DrawPolygon(sf::RenderWindow& window, const Polygon& poly, sf::Color fillColor,
		sf::Color outlineColor) const;

	// Реализация механизмов разбиения многоугольника на грани
	Polygons ConvertFacesToPoly(const SuperFaces& viewFaces) const;
	void SplitFaces(
		size_t i, size_t j, SuperFaces& faces, SuperEdges& diagonals, size_t& currIdx);
	SuperEdges FindIntersectionsAndSplit(
		const SuperEdge& edge, SuperEdges& diagonals, size_t& currIdx, SuperFaces& faces);
	void UpdateFaces(const SuperEdges& partsEdge, SuperFaces& faces);

	// Новый
	bool HandleRaycasting(const VertexRef& startVertex, const Point& directionPoint,
		SuperFaces& faces, size_t& currIdx, SuperEdge& outRaycastEdge);
	SuperEdges CreateSegmentsFromPoints(const SuperFace& vertices);
	HistoryPoly GetFans(const Polygons& viewFaces);

private:
	Polygon m_outer;
	Polygons m_holes;

	// SFML
	sf::Font m_font;
	sf::Text m_text;
	sf::Color m_outerFill = color::GRAY;
	sf::Color m_outerOutline = color::BLACK;
	sf::Color m_holeFill = color::PINK;
	sf::Color m_holeOutline = color::PURPLE;
};

#endif // POLYGON_H