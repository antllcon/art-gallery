#ifndef GEOMETRY_UTILS_H
#define GEOMETRY_UTILS_H

#include "../types.hpp"
#include <cmath>
#include <unordered_set>

struct PointComparer
{
	bool operator()(const Point& a, const Point& b) const
	{
		if (std::abs(a.first - b.first) > 1e-9)
		{
			return a.first < b.first;
		}
		return a.second < b.second;
	}
};

struct PointHash
{
	size_t operator()(const Point& p) const
	{
		return std::hash<double>{}(p.first) ^ std::hash<double>{}(p.second);
	}
};

struct PointEqual
{
	bool operator()(const Point& a, const Point& b) const
	{
		return a.first == b.first && a.second == b.second;
	}
};

namespace GeometryUtils
{
void RemoveDuplicates(Polygon& polygon);
bool IsPointInPolygon(const Polygon& poly, const Point& a);
bool IsPolygonInside(const Polygon& outer, const Polygon& inner);
bool IsSelfIntersections(const Polygon& poly);
bool IsBoundBox(const Edge& a, const Edge& b);
bool IsSimplePolygon(const Polygon& poly);
bool AreEdgesInteract(const Edge& a, const Edge& b);
bool ArePointsOpposite(const Point& a, const Point& b, const Point& c, const Point& d);
bool IsPointInAngle(const Point& D, const Point& A, const Point& C, const Point& B);
bool IsPointsVisibility(const Point& a, const Point& b, const Edges& edges,
	const Polygon& outer, const Polygons& holes);
bool IsDiagonalValid(const Polygon& poly, size_t i, size_t j);
bool GetSegmentsIntersection(const Point& a1, const Point& a2, const Point& b1,
	const Point& b2, Point& intersection);
bool RaySegmentIntersection(const Point& rayStart, const Point& rayDirPoint,
	const Point& segA, const Point& segB, Point& point);
bool AreNeighbors(size_t i, size_t j, size_t n);
bool PointsAreEqualGeometrically(const Point& a, const Point& b);
void SortVerticesAlongEdge(std::vector<VertexRef>& vertices, const Point& startPoint);

// Старая реализация
void SortPointsAlongEdge(std::vector<Point>& points, const Point& startPoint);
double CrossProduct(const Point& a, const Point& b, const Point& c);
} // namespace GeometryUtils

#endif // GEOMETRY_UTILS_H