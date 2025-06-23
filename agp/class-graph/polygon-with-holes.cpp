#include "polygon-with-holes.h"
#include "../class-set-cover/cover.hpp"
#include "../class-triangulator/triangulator.h"
#include "../geom-utils/geom-utils.h"
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace GeometryUtils;

namespace
{
bool AssertIsValidShape(const Polygon& shape)
{
	if (shape.size() < 3)
	{
		throw std::runtime_error("Не хватает точек");
		return false;
	}

	if (IsSelfIntersections(shape))
	{
		throw std::runtime_error("Не должно быть самопересечений");
		return false;
	}

	return true;
}
}; // namespace

bool operator==(const VertexRef& lhs, const VertexRef& rhs)
{
	return lhs.index == rhs.index;
}

PolygonWithHoles::PolygonWithHoles(const Polygon& outer, const Polygons& holes)
	: m_holes(holes)
	, m_outer(outer)
{
}

const Polygon& PolygonWithHoles::GetOuter() const
{
	return m_outer;
}

const Polygons& PolygonWithHoles::GetHoles() const
{
	return m_holes;
}

size_t PolygonWithHoles::GetCountPoints() const
{
	int holesPoints = 0;
	for (const auto& hole : m_holes)
	{
		holesPoints += hole.size();
	}
	return m_outer.size() + holesPoints;
}

Points PolygonWithHoles::GetPoints() const
{
	Points points = m_outer;

	for (const auto& hole : m_holes)
	{
		size_t n = hole.size();
		for (size_t i = 0; i < n; ++i)
		{
			points.emplace_back(hole[i]);
		}
	}

	return points;
}

Edges PolygonWithHoles::GetEdges() const
{
	Edges edges;

	size_t n = m_outer.size();
	for (size_t i = 0; i < n; ++i)
	{
		edges.emplace_back(m_outer[i], m_outer[(i + 1) % n]);
	}

	for (const auto& hole : m_holes)
	{
		size_t m = hole.size();
		for (size_t j = 0; j < m; ++j)
		{
			edges.emplace_back(hole[j], hole[(j + 1) % m]);
		}
	}

	return edges;
}

bool PolygonWithHoles::IsOuter() const
{
	return !m_outer.empty();
}

bool PolygonWithHoles::IsHoles() const
{
	return !m_holes.empty();
}

bool PolygonWithHoles::AddOuter(const Polygon& shape)
{
	if (!AssertIsValidShape(shape))
	{
		return false;
	}

	Polygon shapeCopy = shape;
	RemoveDuplicates(shapeCopy);

	for (const auto& point : shapeCopy)
	{
		m_outer.emplace_back(point.first, point.second);
	}
	return true;
}

bool PolygonWithHoles::AddHole(const Polygon& hole)
{
	if (m_outer.empty())
	{
		throw std::runtime_error("Нет внешнего контура");
		return false;
	}

	if (!AssertIsValidShape(hole))
	{
		return false;
	}

	Polygon holeCopy = hole;
	RemoveDuplicates(holeCopy);

	if (!IsPolygonInside(m_outer, holeCopy))
	{
		throw std::runtime_error("Отверстие выходит за пределы внешнего контура");
		return false;
	}

	m_holes.emplace_back(holeCopy);
	return true;
}

void PolygonWithHoles::Print() const
{
	size_t enter = 3;

	std::cout << "================" << std::endl;
	std::cout << "Полигон" << (IsSimplePolygon(m_outer) ? " (простой)" : " (сложный)")
			  << std::endl;
	for (const auto& point : m_outer)
	{
		std::cout << "[" << point.first << ", " << point.second << "] ";
		if (--enter == 0)
		{
			std::cout << std::endl;
			enter = 3;
		}
	}
	std::cout << std::endl;

	enter = 3;

	std::cout << "Отверстия: (" << m_holes.size() << ") шт." << std::endl;
	for (const auto& hole : m_holes)
	{
		std::cout << (IsSimplePolygon(hole) ? " (простой) - " : " (сложный) - ");
		for (const auto& point : hole)
		{
			std::cout << "[" << point.first << ", " << point.second << "] ";
		}
		std::cout << std::endl;
	}
	std::cout << "================" << std::endl;
}

void PolygonWithHoles::Draw(sf::RenderWindow& window) const
{
	DrawPolygon(window, m_outer, m_outerFill, m_outerOutline);
	for (const auto& hole : m_holes)
		DrawPolygon(window, hole, m_holeFill, m_holeOutline);
}

Points PolygonWithHoles::CreatePointsGuards()
{
	auto visibilityComponents = GetViewFaces();
	auto fans = GetFans(visibilityComponents);
	if (visibilityComponents.empty() || fans.empty())
		return {};

	vector<unordered_set<int>> setsForCover;
	auto allPoints = GetPoints();

	int componentCounter = 0;
	unordered_map<size_t, int> componentToIndex;

	for (size_t vertexIdx = 0; vertexIdx < fans.size(); ++vertexIdx)
	{
		unordered_set<int> visibleComponents;
		const auto& fan = fans[vertexIdx];
		for (const auto& component : fan)
		{
			size_t componentHash = 0;
			for (const auto& point : component)
			{
				componentHash ^= std::hash<double>{}(point.first);
				componentHash ^= std::hash<double>{}(point.second);
			}

			if (componentToIndex.find(componentHash) == componentToIndex.end())
				componentToIndex[componentHash] = componentCounter++;

			visibleComponents.insert(componentToIndex[componentHash]);
		}

		setsForCover.push_back(visibleComponents);
	}

	GreedySetCover<int> solver(setsForCover);
	auto guardIndices = solver.solve();

	Points guards;
	for (auto index : guardIndices)
		if (index < allPoints.size())
			guards.push_back(allPoints[index]);

	return guards;
}

HistoryPoly PolygonWithHoles::GetFans(const Polygons& viewFaces)
{
	HistoryPoly fans;
	auto points = GetPoints();
	auto edges = GetEdges();

	for (size_t i = 0; i < points.size(); ++i)
	{
		const auto& vertex = points[i];
		Polygons visibleComponents;

		for (const auto& component : viewFaces)
		{
			bool isVisible = true;

			if (!component.empty())
			{
				isVisible
					= IsPointsVisibility(vertex, component[0], edges, m_outer, m_holes);
			}

			if (isVisible)
			{
				visibleComponents.push_back(component);
			}
		}

		fans.push_back(visibleComponents);
	}

	return fans;
}

Polygons PolygonWithHoles::GetViewFaces()
{
	if (m_holes.empty() && IsSimplePolygon(m_outer))
	{
		return { m_outer };
	}

	auto points = GetPoints();
	auto currIdx = GetCountPoints() - 1;

	SuperFace faceIndices;
	for (size_t i = 0; i < points.size(); ++i)
		faceIndices.emplace_back(i, points[i]);

	SuperFaces viewFaces = { faceIndices };
	HistoryPoly monster;
	SuperEdges diagonals;

	for (size_t i = 0; i < GetCountPoints(); ++i)
	{
		const Point first = points[i];
		for (size_t j = i + 1; j < GetCountPoints(); ++j)
		{
			const Point second = points[j];

			if (first == second)
				continue;

			if (!IsPointsVisibility(first, second, GetEdges(), m_outer, m_holes))
				continue;

			SplitFaces(i, j, viewFaces, diagonals, currIdx);
			monster.emplace_back(ConvertFacesToPoly(viewFaces));
		}
	}

	return monster.back();
}

HistoryPoly PolygonWithHoles::CreateVisualFaces()
{
	auto points = GetPoints();
	auto currIdx = GetCountPoints() - 1;

	SuperFace faceIndices;
	for (size_t i = 0; i < points.size(); ++i)
	{
		faceIndices.emplace_back(i, points[i]);
	}
	SuperFaces viewFaces = { faceIndices };

	HistoryPoly monster;
	SuperEdges diagonals;

	for (size_t i = 0; i < GetCountPoints(); ++i)
	{
		const Point first = points[i];
		for (size_t j = i + 1; j < GetCountPoints(); ++j)
		{
			const Point second = points[j];

			if (first == second)
				continue;

			if (!IsPointsVisibility(first, second, GetEdges(), m_outer, m_holes))
				continue;

			SplitFaces(i, j, viewFaces, diagonals, currIdx);
			monster.emplace_back(ConvertFacesToPoly(viewFaces));
		}
	}

	return monster;
}

Polygons PolygonWithHoles::ConvertFacesToPoly(const SuperFaces& viewFaces) const
{
	Polygons polygons;
	for (const auto& viewFace : viewFaces)
	{
		Polygon face;
		for (size_t i = 0; i < viewFace.size(); i++)
			face.push_back(viewFace[i].point);
		polygons.push_back(face);
	}
	return polygons;
}

std::optional<HitInfo> FindClosestHitOnBoundary(
	const Point& start, const Point& direction, const Edges& boundary)
{
	std::optional<HitInfo> closestHit = std::nullopt;
	double minDistanceSq = std::numeric_limits<double>::max();

	for (const auto& edge : boundary)
	{
		Point interPoint;
		if (RaySegmentIntersection(direction, start, edge.first, edge.second, interPoint))
		{
			double dx = interPoint.first - start.first;
			double dy = interPoint.second - start.second;
			double distSq = dx * dx + dy * dy;

			if (distSq < minDistanceSq)
			{
				minDistanceSq = distSq;
				closestHit
					= HitInfo{ interPoint, { { 0, edge.first }, { 0, edge.second } } };
			}
		}
	}
	return closestHit;
}

bool IsPointOnSegment(
	const Point& p, const Point& a, const Point& b, double epsilon = 1e-9)
{
	// 1. Проверка коллинеарности через векторное произведение
	double cross_product = GeometryUtils::CrossProduct(a, b, p);
	if (std::abs(cross_product) > epsilon)
		return false;

	// 2. Проверка принадлежности "коробке" отрезка
	bool within_x = (p.first >= std::min(a.first, b.first) - epsilon)
		&& (p.first <= std::max(a.first, b.first) + epsilon);
	bool within_y = (p.second >= std::min(a.second, b.second) - epsilon)
		&& (p.second <= std::max(a.second, b.second) + epsilon);

	return within_x && within_y;
}

bool UpdateFaceByPoint(const VertexRef& newVertex, SuperFaces& faces)
{
	bool updated_at_least_one = false;
	for (auto& face : faces)
	{
		const auto n = face.size();
		for (size_t i = 0; i < n; ++i)
		{
			const VertexRef& vFirst = face[i];
			const VertexRef& vSecond = face[(i + 1) % n];

			if (IsPointOnSegment(newVertex.point, vFirst.point, vSecond.point))
			{
				auto insertionIterator = face.begin() + i + 1;
				face.insert(insertionIterator, newVertex);

				updated_at_least_one = true;
				break;
			}
		}
	}

	return updated_at_least_one;
}

bool PolygonWithHoles::HandleRaycasting(const VertexRef& startVertex,
	const Point& directionPoint, SuperFaces& faces, size_t& currIdx,
	SuperEdge& outRaycastEdge)
{
	auto hitInfoOpt
		= FindClosestHitOnBoundary(startVertex.point, directionPoint, GetEdges());

	if (hitInfoOpt)
	{
		const auto& hitInfo = *hitInfoOpt;
		VertexRef newVertexOnBoundary(++currIdx, hitInfo.interPoint);

		if (UpdateFaceByPoint(newVertexOnBoundary, faces))
		{
			outRaycastEdge = SuperEdge(startVertex, newVertexOnBoundary);
			return true;
		}
		else
		{
			throw std::runtime_error(
				"Не удалось найти ребро в топологии для точки пересечения");
		}
	}
	return false;
}

void PolygonWithHoles::SplitFaces(
	size_t i, size_t j, SuperFaces& faces, SuperEdges& diagonals, size_t& currIdx)
{
	auto n = GetCountPoints();
	auto points = GetPoints();

	const VertexRef vertexI(i, points[i]);
	const VertexRef vertexJ(j, points[j]);

	const Point a = points[i];
	const Point b = points[j];

	// 1. Создаем ребро(а) видимости
	SuperEdges initEdges;
	initEdges.emplace_back(vertexI, vertexJ);

	if (!IsPointInAngle(points[(i + n - 1) % n], a, points[(i + 1) % n], b))
	{
		SuperEdge raycastEdge;
		if (HandleRaycasting(vertexI, b, faces, currIdx, raycastEdge))
		{
			initEdges.emplace_back(raycastEdge);
		}
	}

	if (!IsPointInAngle(points[(j + n - 1) % n], b, points[(j + 1) % n], a))
	{
		SuperEdge raycastEdge;
		if (HandleRaycasting(vertexJ, a, faces, currIdx, raycastEdge))
		{
			initEdges.emplace_back(raycastEdge);
		}
	}

	// 2. Обрабатываем их пересечения с предыдущими разрезами
	SuperEdges finalSegments;
	for (const auto& edge : initEdges)
	{
		auto newSegmentes = FindIntersectionsAndSplit(edge, diagonals, currIdx, faces);
		finalSegments.insert(
			finalSegments.end(), newSegmentes.begin(), newSegmentes.end());
	}

	// 3. Разрезаем грани
	diagonals.insert(diagonals.end(), finalSegments.begin(), finalSegments.end());
	UpdateFaces(finalSegments, faces);
}

SuperEdges PolygonWithHoles::FindIntersectionsAndSplit(
	const SuperEdge& edge, SuperEdges& diagonals, size_t& currIdx, SuperFaces& faces)
{
	std::vector<IntersectionInfo> intersections;

	std::vector<VertexRef> breakPoints;
	breakPoints.push_back(edge.first);
	breakPoints.push_back(edge.second);

	for (const auto& diagonal : diagonals)
	{
		Point intersectionPoint;
		if (GeometryUtils::GetSegmentsIntersection(edge.first.point, edge.second.point,
				diagonal.first.point, diagonal.second.point, intersectionPoint))
		{
			if (GeometryUtils::PointsAreEqualGeometrically(
					intersectionPoint, edge.first.point)
				|| GeometryUtils::PointsAreEqualGeometrically(
					intersectionPoint, edge.second.point)
				|| GeometryUtils::PointsAreEqualGeometrically(
					intersectionPoint, diagonal.first.point)
				|| GeometryUtils::PointsAreEqualGeometrically(
					intersectionPoint, diagonal.second.point))
			{
				continue;
			}
			intersections.push_back({ intersectionPoint, diagonal });
			breakPoints.push_back(VertexRef(0, intersectionPoint));
		}
	}

	std::sort(breakPoints.begin(), breakPoints.end(),
		[](const VertexRef& a, const VertexRef& b) {
			return a.point.first < b.point.first
				|| (a.point.first == b.point.first && a.point.second < b.point.second);
		});
	breakPoints.erase(std::unique(breakPoints.begin(), breakPoints.end(),
						  [](const VertexRef& a, const VertexRef& b) {
							  return GeometryUtils::PointsAreEqualGeometrically(
								  a.point, b.point);
						  }),
		breakPoints.end());

	SortVerticesAlongEdge(breakPoints, edge.first.point);

	SuperFace finalBreakPoints;
	std::map<Point, VertexRef, PointComparer> newVerticesMap;

	for (const auto& vRef : breakPoints)
	{
		if (GeometryUtils::PointsAreEqualGeometrically(vRef.point, edge.first.point))
		{
			finalBreakPoints.push_back(edge.first);
		}
		else if (GeometryUtils::PointsAreEqualGeometrically(
					 vRef.point, edge.second.point))
		{
			finalBreakPoints.push_back(edge.second);
		}
		else
		{
			VertexRef newVertex(++currIdx, vRef.point);
			finalBreakPoints.push_back(newVertex);
			newVerticesMap.emplace(vRef.point, newVertex);
		}
	}

	if (intersections.empty())
	{
		return CreateSegmentsFromPoints(finalBreakPoints);
	}

	std::map<std::pair<size_t, size_t>, std::vector<VertexRef>> diagonalToNewVertices;
	for (const auto& info : intersections)
	{
		if (newVerticesMap.count(info.point))
		{
			auto key = std::minmax(info.edge.first.index, info.edge.second.index);
			diagonalToNewVertices[key].push_back(newVerticesMap.at(info.point));
		}
	}

	SuperFaces updatedFaces;
	updatedFaces.reserve(faces.size());
	for (const auto& face : faces)
	{
		SuperFace newFaceVertices;
		newFaceVertices.reserve(face.size() + diagonalToNewVertices.size());

		for (size_t i = 0; i < face.size(); ++i)
		{
			const auto& p1 = face[i];
			const auto& p2 = face[(i + 1) % face.size()];

			newFaceVertices.push_back(p1);

			auto key = std::minmax(p1.index, p2.index);
			if (diagonalToNewVertices.count(key))
			{
				std::vector<VertexRef> currentEdgeNewVertices
					= diagonalToNewVertices.at(key);

				currentEdgeNewVertices.insert(currentEdgeNewVertices.begin(), p1);
				currentEdgeNewVertices.push_back(p2);

				SortVerticesAlongEdge(currentEdgeNewVertices, p1.point);

				for (const auto& newVtx : currentEdgeNewVertices)
				{
					if (!GeometryUtils::PointsAreEqualGeometrically(
							newVtx.point, p1.point)
						&& !GeometryUtils::PointsAreEqualGeometrically(
							newVtx.point, p2.point))
					{
						newFaceVertices.push_back(newVtx);
					}
				}
			}
		}
		updatedFaces.push_back(newFaceVertices);
	}

	faces = updatedFaces;

	SuperEdges updatedDiagonals;
	updatedDiagonals.reserve(diagonals.size());
	for (const auto& currentDiagonal : diagonals)
	{
		auto key = std::minmax(currentDiagonal.first.index, currentDiagonal.second.index);

		if (diagonalToNewVertices.count(key))
		{
			std::vector<VertexRef> newVerticesOnThisDiagonal
				= diagonalToNewVertices.at(key);

			newVerticesOnThisDiagonal.insert(
				newVerticesOnThisDiagonal.begin(), currentDiagonal.first);
			newVerticesOnThisDiagonal.push_back(currentDiagonal.second);

			SortVerticesAlongEdge(newVerticesOnThisDiagonal, currentDiagonal.first.point);

			SuperEdges segmentsForThisDiagonal
				= CreateSegmentsFromPoints(newVerticesOnThisDiagonal);
			updatedDiagonals.insert(updatedDiagonals.end(),
				segmentsForThisDiagonal.begin(), segmentsForThisDiagonal.end());
		}
		else
		{
			updatedDiagonals.push_back(currentDiagonal);
		}
	}
	diagonals = updatedDiagonals;

	return CreateSegmentsFromPoints(finalBreakPoints);
}

SuperEdges PolygonWithHoles::CreateSegmentsFromPoints(const SuperFace& vertices)
{
	SuperEdges segments;
	if (vertices.size() < 2)
		return segments;

	for (size_t i = 0; i < vertices.size() - 1; ++i)
	{
		segments.emplace_back(vertices[i], vertices[i + 1]);
	}
	return segments;
}

void PolygonWithHoles::UpdateFaces(const SuperEdges& partsEdge, SuperFaces& faces)
{
	for (const auto& edge : partsEdge)
	{
		const auto a = edge.first;
		const auto b = edge.second;

		for (auto itFace = faces.begin(); itFace != faces.end();)
		{
			SuperFace& face = *itFace;

			auto itA = std::find(face.begin(), face.end(), a);
			auto itB = std::find(face.begin(), face.end(), b);

			if (itA == face.end() || itB == face.end())
			{
				++itFace;
				continue;
			}

			if (itA > itB)
			{
				std::swap(itA, itB);
			}

			SuperFace faceRight(itA, itB + 1);
			SuperFace faceLeft(itB, face.end());
			faceLeft.insert(faceLeft.end(), face.begin(), itA + 1);

			if (faceRight.size() < 3 || faceLeft.size() < 3)
			{
				// throw std::runtime_error("Invalid face after split");
			}

			itFace = faces.erase(itFace);
			faces.insert(itFace, { faceRight, faceLeft });
			break;
		}
	}
}

float PolygonWithHoles::FindCollision(
	const Point& origin, float angle, float length) const
{
	const double dx = std::cos(angle);
	const double dy = std::sin(angle);
	const double EPS = 1e-4;

	double startX = origin.first + dx * EPS;
	double startY = origin.second + dy * EPS;

	Point start = { startX, startY };
	if (!IsPointInPolygon(m_outer, start))
	{
		return 0;
	}

	for (const auto& hole : m_holes)
	{
		if (IsPointInPolygon(hole, start))
		{
			return 0;
		}
	}

	double endX = startX + dx * length;
	double endY = startY + dy * length;

	Edge line = { { startX, startY }, { endX, endY } };

	float minDist = length;
	bool found = false;

	auto checkEdge = [&](const Point& p1, const Point& p2) {
		double x1 = startX, y1 = startY;
		double x2 = endX, y2 = endY;
		double x3 = p1.first, y3 = p1.second;
		double x4 = p2.first, y4 = p2.second;

		double denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
		if (std::abs(denom) < 1e-12)
		{
			return;
		}

		double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom;
		double u = -((x1 - x2) * (y1 - y3) - (y1 - y2) * (x1 - x3)) / denom;

		if (t >= 0 && u >= 0 && u <= 1)
		{
			double intersectX = x1 + t * (x2 - x1);
			double intersectY = y1 + t * (y2 - y1);
			double dist
				= std::hypot(intersectX - origin.first, intersectY - origin.second);
			if (dist < minDist)
			{
				minDist = dist;
				found = true;
			}
		}
	};

	for (size_t i = 0; i < m_outer.size(); ++i)
	{
		const Point& p1 = m_outer[i];
		const Point& p2 = m_outer[(i + 1) % m_outer.size()];
		Edge outerLine = { p1, p2 };
		if (AreEdgesInteract(line, outerLine))
		{
			checkEdge(p1, p2);
		}
	}

	for (const auto& hole : m_holes)
	{
		for (size_t i = 0; i < hole.size(); ++i)
		{
			const Point& p1 = hole[i];
			const Point& p2 = hole[(i + 1) % hole.size()];
			Edge holeLine = { p1, p2 };
			if (AreEdgesInteract(line, holeLine))
			{
				checkEdge(p1, p2);
			}
		}
	}

	return minDist;
}

// TODO: есть что переделать
void PolygonWithHoles::DrawPolygon(sf::RenderWindow& window, const Polygon& poly,
	sf::Color fillColor, sf::Color outlineColor) const
{
	// Триангулируем
	PolygonTriangulator triangulator;
	PolygonTriangulator::Polygon_2 outer;
	for (const auto& point : poly)
	{
		outer.push_back(PolygonTriangulator::Point(point.first, point.second));
	}
	triangulator.SetOuterPolygon(outer);
	triangulator.Triangulate();

	// Перевод в sfml
	sf::VertexArray outerTriangles(sf::Triangles);
	for (const auto& triangle : triangulator.GetTriangles())
	{
		const auto& [p0, p1, p2] = triangle;
		outerTriangles.append(sf::Vertex(sf::Vector2f(p0.x(), p0.y()), fillColor));
		outerTriangles.append(sf::Vertex(sf::Vector2f(p1.x(), p1.y()), fillColor));
		outerTriangles.append(sf::Vertex(sf::Vector2f(p2.x(), p2.y()), fillColor));
	}
	window.draw(outerTriangles);

	// // Вынести инициализацию отдельно!
	// // Отрисовка вершин
	// sf::CircleShape circle;
	// circle.setRadius(constants::RADIUS);
	// circle.setFillColor(color::WHITE);
	// circle.setOutlineColor(color::GRAY);
	// circle.setOutlineThickness(constants::THICKNESS);

	// sf::Font font;
	// sf::Text text;

	// // Загрузка шрифта
	// if (!font.loadFromFile(fonts::CASCADILIA_MONO))
	// {
	// 	throw std::runtime_error("Не удалось загрузить шрифт");
	// }

	// // Формат текста
	// text.setFont(font);
	// text.setCharacterSize(text::SIZE);
	// text.setFillColor(color::BLACK);

	// auto vertices = GetPoints();
	// for (size_t i = 0; i < vertices.size(); ++i)
	// {
	// 	const auto& vertex = vertices[i];
	// 	float x = static_cast<float>(vertex.first);
	// 	float y = static_cast<float>(vertex.second);

	// 	// Отрисовка кружка
	// 	circle.setPosition(x - circle.getRadius(), y - circle.getRadius());
	// 	window.draw(circle);

	// 	// Отрисовка номера вершины
	// 	text.setString(std::to_string(i));

	// 	// Центрирование текста
	// 	sf::FloatRect textBounds = text.getLocalBounds();
	// 	text.setOrigin(textBounds.left + textBounds.width / 2.0f,
	// 		textBounds.top + textBounds.height / 2.0f);
	// 	text.setPosition(x, y);

	// 	window.draw(text);
	// }
}