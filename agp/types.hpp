#ifndef TYPES_HPP
#define TYPES_HPP

#include <utility>
#include <vector>

using Point = std::pair<double, double>;
using Points = std::vector<Point>;

using Edge = std::pair<Point, Point>;
using Edges = std::vector<Edge>;

using Polygon = Points;
using Polygons = std::vector<Polygon>;
using HistoryPoly = std::vector<Polygons>;

struct VertexRef
{
	size_t index; // Индекс вершины в исходном массиве
	Point point;  // Координаты вершины

	VertexRef() = default;

	VertexRef(size_t idx, const Point& pt)
		: index(idx)
		, point(pt)
	{
	}
};

using SuperFace = std::vector<VertexRef>;
using SuperFaces = std::vector<SuperFace>;
using SuperEdge = std::pair<VertexRef, VertexRef>;
using SuperEdges = std::vector<SuperEdge>;

// Структура для хранения полной информации о каждом пересечении
struct IntersectionInfo
{
	Point point; // Координаты точки пересечения
	SuperEdge edge; // Диагональ, с которой произошло пересечение
};

// Вспомогательная структура для хранения результата пересечения
struct HitInfo
{
	Point interPoint;	 // Точка пересечения
	SuperEdge interEdge; // Ребро, с которым пересеклись
};

#endif // TYPES_HPP