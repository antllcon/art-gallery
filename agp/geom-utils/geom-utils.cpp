#include "geom-utils.h"
#include <algorithm>
#include <optional>
#include <vector>

// TODO: Тесты
void GeometryUtils::RemoveDuplicates(Polygon& polygon)
{
	std::unordered_set<Point, PointHash, PointEqual> seen;
	auto newEnd = std::remove_if(polygon.begin(), polygon.end(),
		[&seen](const Point& p) { return !seen.insert(p).second; });
	polygon.erase(newEnd, polygon.end());
}

// Ищет точки внутри территории полигона, граница и вершны - не в счет
bool GeometryUtils::IsPointInPolygon(const Polygon& poly, const Point& a)
{
	bool isInside = false;
	size_t n = poly.size();
	for (size_t i = 0, j = n - 1; i < n; j = i++)
	{
		double xi = poly[i].first, yi = poly[i].second;
		double xj = poly[j].first, yj = poly[j].second;

		// Игнорируем горизонтальные рёбра
		if (yi == yj)
			continue;

		// Проверка пересечения луча с ребром
		if ((yi > a.second) != (yj > a.second))
		{
			double xIntersect = (xj - xi) * (a.second - yi) / (yj - yi) + xi;
			if (a.first <= xIntersect)
			{
				isInside = !isInside;
			}
		}
	}
	return isInside;
}

bool GeometryUtils::IsPolygonInside(const Polygon& outer, const Polygon& inner)
{
	for (const auto& point : inner)
	{
		if (!IsPointInPolygon(outer, point))
			return false;
	}

	size_t nInner = inner.size();
	size_t nOuter = outer.size();
	for (size_t i = 0; i < nInner; ++i)
	{
		Edge innerEdge = { inner[i], inner[(i + 1) % nInner] };
		for (size_t j = 0; j < nOuter; ++j)
		{
			Edge outerEdge = { outer[j], outer[(j + 1) % nOuter] };
			if (AreEdgesInteract(innerEdge, outerEdge))
				return false;
		}
	}

	return true;
}

bool GeometryUtils::AreEdgesInteract(const Edge& a, const Edge& b)
{
	if (!IsBoundBox(a, b))
	{
		return false;
	}

	bool isCrossingFirst = ArePointsOpposite(a.first, a.second, b.first, b.second);
	bool isCrossingSecond = ArePointsOpposite(b.first, b.second, a.first, a.second);

	return isCrossingFirst && isCrossingSecond;
}

bool GeometryUtils::IsBoundBox(const Edge& a, const Edge& b)
{
	auto aMinX = std::min(a.first.first, a.second.first);
	auto aMaxX = std::max(a.first.first, a.second.first);
	auto aMinY = std::min(a.first.second, a.second.second);
	auto aMaxY = std::max(a.first.second, a.second.second);

	auto bMinX = std::min(b.first.first, b.second.first);
	auto bMaxX = std::max(b.first.first, b.second.first);
	auto bMinY = std::min(b.first.second, b.second.second);
	auto bMaxY = std::max(b.first.second, b.second.second);

	return (aMaxX >= bMinX) && (bMaxX >= aMinX) && (aMaxY >= bMinY) && (bMaxY >= aMinY);
}

bool GeometryUtils::ArePointsOpposite(
	const Point& a, const Point& b, const Point& c, const Point& d)
{
	auto crossFirst = CrossProduct(c, d, a);
	auto crossSecond = CrossProduct(c, d, b);

	return (crossSecond * crossFirst) < 0;
}

double GeometryUtils::CrossProduct(const Point& a, const Point& b, const Point& c)
{
	return (b.first - a.first) * (c.second - a.second)
		- (b.second - a.second) * (c.first - a.first);
}

// TODO: Тесты
bool GeometryUtils::IsSimplePolygon(const Polygon& poly)
{
	bool got_negative = false;
	bool got_positive = false;
	size_t n = poly.size();

	for (size_t i = 0; i < n; ++i)
	{
		const Point& a = poly[i];
		const Point& b = poly[(i + 1) % n];
		const Point& c = poly[(i + 2) % n];

		auto cross = CrossProduct(a, b, c);

		if (cross < 0)
		{
			got_negative = true;
		}
		else if (cross > 0)
		{
			got_positive = true;
		}

		if (got_negative && got_positive)
		{
			return false;
		}
	}
	return true;
}

bool GeometryUtils::IsPointInAngle(
	const Point& D, const Point& A, const Point& C, const Point& B)
{
	// Если B совпадает с вершиной угла (A) или с одной из сторон (C или D),
	// считаем, что точка лежит в угле.
	if (B == A || B == C || B == D)
		return false;

	double cp1 = CrossProduct(A, C, B);
	double cp2 = CrossProduct(A, D, B);

	auto sgn = [](double x) {
		const double eps = 1e-9;
		if (x > eps)
			return 1;
		if (x < -eps)
			return -1;
		return 0;
	};

	int s1 = sgn(cp1);
	int s2 = sgn(cp2);

	return s1 * s2 <= 0;
}

bool GeometryUtils::IsPointsVisibility(const Point& a, const Point& b, const Edges& edges,
	const Polygon& outer, const Polygons& holes)
{
	for (const auto& edge : edges)
	{
		if (edge.first == a || edge.second == a || edge.first == b || edge.second == b)
			continue;

		Edge line = { a, b };

		if (AreEdgesInteract(line, edge))
			return false;

		Point midpoint = { (a.first + b.first) / 2, (a.second + b.second) / 2 };

		if (!IsPointInPolygon(outer, midpoint))
			return false;

		for (const auto& hole : holes)
			if (IsPointInPolygon(hole, midpoint))
				return false;
	}
	return true;
}

bool GeometryUtils::IsSelfIntersections(const Polygon& poly)
{
	size_t n = poly.size();
	for (size_t i = 0; i < n; ++i)
	{
		Edge e1 = { poly[i], poly[(i + 1) % n] };
		for (size_t j = i + 1; j < n; ++j)
		{
			if (j == i || (j + 1) % n == i || (i + 1) % n == j)
				continue;

			Edge e2 = { poly[j], poly[(j + 1) % n] };
			if (AreEdgesInteract(e1, e2))
				return true;
		}
	}

	return false;
}

// TODO: написать тесты (частично не работает)
bool GeometryUtils::IsDiagonalValid(const Polygon& poly, size_t i, size_t j)
{
	size_t n = poly.size();
	Edge diag = { poly[i], poly[j] };

	// Диагональ не должна совпадать с ребром
	if ((i + 1) % n == j || (j + 1) % n == i)
		return false;

	// Не должна пересекать другие рёбра
	for (size_t k = 0; k < n; ++k)
	{
		size_t k1 = k;
		size_t k2 = (k + 1) % n;
		if (k1 == i || k2 == i || k1 == j || k2 == j)
			continue;
		Edge edge = { poly[k1], poly[k2] };
		if (AreEdgesInteract(diag, edge))
			return false;
	}
	// Диагональ должна лежать внутри многоугольника
	double mx = (poly[i].first + poly[j].first) / 2.0;
	double my = (poly[i].second + poly[j].second) / 2.0;
	if (!IsPointInPolygon(poly, { mx, my }))
		return false;

	return true;
}

bool GeometryUtils::GetSegmentsIntersection(const Point& a1, const Point& a2,
	const Point& b1, const Point& b2, Point& intersection)
{
	double x1 = a1.first, y1 = a1.second;
	double x2 = a2.first, y2 = a2.second;
	double x3 = b1.first, y3 = b1.second;
	double x4 = b2.first, y4 = b2.second;

	double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

	// Параллельные или коллинеарные отрезки
	if (std::abs(denom) < 1e-9) // Увеличенный эпсилон для проверки параллельности
		return false;

	double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;
	double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denom;

	// Проверяем, находится ли точка пересечения СТРОГО внутри обоих отрезков
	// (точки на концах отрезков обрабатываются отдельно в вызывающей функции)
	const double EPS = 1e-9;
	if (t > EPS && t < 1.0 - EPS && u > EPS && u < 1.0 - EPS)
	{
		intersection.first = x1 + t * (x2 - x1);
		intersection.second = y1 + t * (y2 - y1);
		return true;
	}

	return false;
}

// bool GeometryUtils::GetSegmentsIntersection(const Point& a1, const Point& a2,
// 	const Point& b1, const Point& b2, Point& intersection)
// {
// 	Edge edgeA = { a1, a2 };
// 	Edge edgeB = { b1, b2 };

// 	if (!AreEdgesInteract(edgeA, edgeB))
// 		return false;

// 	double x1 = a1.first, y1 = a1.second;
// 	double x2 = a2.first, y2 = a2.second;
// 	double x3 = b1.first, y3 = b1.second;
// 	double x4 = b2.first, y4 = b2.second;

// 	double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
// 	if (std::abs(denom) < 1e-12)
// 		return false; // Отрезки параллельны или совпадают

// 	double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;

// 	intersection.first = x1 + t * (x2 - x1);
// 	intersection.second = y1 + t * (y2 - y1);
// 	return true;
// }

// Проверяет, пересекает ли луч (исходящий из точки rayStart
// в направлении rayDir) отрезок [segA, segB].
bool GeometryUtils::RaySegmentIntersection(const Point& rayStart,
	const Point& rayDirPoint, const Point& segA, const Point& segB, Point& point)
{
	// Направление луча (вектор)
	double dxR = rayDirPoint.first - rayStart.first;
	double dyR = rayDirPoint.second - rayStart.second;

	// Направление отрезка
	double dxS = segB.first - segA.first;
	double dyS = segB.second - segA.second;

	// Вычисляем определитель
	double det = dxR * dyS - dyR * dxS;
	if (std::abs(det) < 1e-9)
		return false; // параллельны или коллинеарны

	double t
		= ((segA.first - rayStart.first) * dyS - (segA.second - rayStart.second) * dxS)
		/ det;
	double u
		= ((segA.first - rayStart.first) * dyR - (segA.second - rayStart.second) * dxR)
		/ det;

	const double EPSILON_RAYCAST = 1e-9;

	// --- ГЛАВНОЕ ИЗМЕНЕНИЕ ЗДЕСЬ ---
	// Условие для луча: t > (1.0 + EPSILON_RAYCAST)
	// Это гарантирует, что точка пересечения находится *после* точки rayDirPoint.
	// Если t = 1, это точка rayDirPoint. Если t < 1, это точка до rayDirPoint.
	// Мы ищем точки только после нее.
	if (t > (1.0 + EPSILON_RAYCAST) && u >= 0 && u <= 1)
	{
		point = { rayStart.first + t * dxR, rayStart.second + t * dyR };
		return true;
	}
	return false;
}

// TODO: нужны тесты
bool GeometryUtils::AreNeighbors(size_t i, size_t j, size_t n)
{
	return (i == (j + 1) % n || j == (i + 1) % n || (i == 0 && j == n - 1)
		|| (j == 0 && i == n - 1));
}

bool GeometryUtils::PointsAreEqualGeometrically(const Point& a, const Point& b)
{
	return std::abs(a.first - b.first) < 1e-9 && std::abs(a.second - b.second) < 1e-9;
}

// Сортировка вершин VertexRef вдоль отрезка, начиная с startPoint.
// Важно: Этот метод предполагает, что все точки уже лежат на отрезке.
void GeometryUtils::SortVerticesAlongEdge(
	std::vector<VertexRef>& vertices, const Point& startPoint)
{
	auto distanceSq = [&startPoint](const Point& p) {
		double dx = p.first - startPoint.first;
		double dy = p.second - startPoint.second;
		return dx * dx + dy * dy;
	};

	std::sort(vertices.begin(), vertices.end(),
		[&distanceSq](const VertexRef& a, const VertexRef& b) {
			return distanceSq(a.point) < distanceSq(b.point);
		});
}

// Старая реализация для точек без доп информации
void GeometryUtils::SortPointsAlongEdge(
	std::vector<Point>& points, const Point& startPoint)
{
	auto distanceSq = [&startPoint](const Point& p) {
		double dx = p.first - startPoint.first;
		double dy = p.second - startPoint.second;
		return dx * dx + dy * dy;
	};

	std::sort(
		points.begin(), points.end(), [&distanceSq](const Point& a, const Point& b) {
			return distanceSq(a) < distanceSq(b);
		});
}