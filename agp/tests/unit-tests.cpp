#include "geom-utils.h"
#include <gtest/gtest.h>
#include <sstream>

// Проверяем, что функция корректно работает с пустым полигоном
TEST(RemoveDuplicatesTest, HandlesEmptyPolygon)
{
	Polygon empty;
	GeometryUtils::RemoveDuplicates(empty);
	EXPECT_TRUE(empty.empty());
}

// Проверяем, что функция не изменяет полигон без дубликатов
TEST(RemoveDuplicatesTest, LeavesUniquePointsUntouched)
{
	Polygon polygon = { { 0, 0 }, { 1, 0 }, { 0, 1 } };
	const Polygon expected = polygon;
	GeometryUtils::RemoveDuplicates(polygon);
	EXPECT_EQ(polygon, expected);
}

// Проверяем удаление последовательных дубликатов
TEST(RemoveDuplicatesTest, RemovesConsecutiveDuplicates)
{
	Polygon polygon = { { 0, 0 }, { 0, 0 }, { 1, 0 }, { 1, 0 }, { 0, 1 } };
	const Polygon expected = { { 0, 0 }, { 1, 0 }, { 0, 1 } };
	GeometryUtils::RemoveDuplicates(polygon);
	EXPECT_EQ(polygon, expected);
}

// Проверяем удаление не-последовательных дубликатов
TEST(RemoveDuplicatesTest, RemovesNonConsecutiveDuplicates)
{
	Polygon polygon = { { 0, 0 }, { 1, 0 }, { 0, 0 }, { 0, 1 }, { 1, 0 } };
	const Polygon expected = { { 0, 0 }, { 1, 0 }, { 0, 1 } };
	GeometryUtils::RemoveDuplicates(polygon);
	EXPECT_EQ(polygon, expected);
}

// Проверяем случай, когда все точки одинаковые
TEST(RemoveDuplicatesTest, HandlesAllPointsSame)
{
	Polygon polygon = { { 1, 1 }, { 1, 1 }, { 1, 1 } };
	const Polygon expected = { { 1, 1 } };
	GeometryUtils::RemoveDuplicates(polygon);
	EXPECT_EQ(polygon, expected);
}

// Проверяем случай с одной точкой (крайний случай)
TEST(RemoveDuplicatesTest, HandlesSinglePoint)
{
	Polygon polygon = { { 5, 5 } };
	const Polygon expected = { { 5, 5 } };
	GeometryUtils::RemoveDuplicates(polygon);
	EXPECT_EQ(polygon, expected);
}

// Проверяем, что порядок оставшихся уникальных точек сохраняется
TEST(RemoveDuplicatesTest, PreservesOrderOfUniquePoints)
{
	Polygon polygon = { { 0, 0 }, { 1, 0 }, { 0, 0 }, { 2, 0 }, { 1, 0 }, { 3, 0 } };
	const Polygon expected = { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 } };
	GeometryUtils::RemoveDuplicates(polygon);
	EXPECT_EQ(polygon, expected);
}

//
TEST(GeometryUtilsTest, RemoveDuplicates)
{
	Polygon polygon = { { 0, 0 }, { 1, 1 }, { 0, 0 }, { 2, 2 }, { 1, 1 }, { 3, 3 } };
	Polygon expected = { { 0, 0 }, { 1, 1 }, { 2, 2 }, { 3, 3 } };

	GeometryUtils::RemoveDuplicates(polygon);

	ASSERT_EQ(polygon.size(), expected.size());
	for (size_t i = 0; i < expected.size(); ++i)
	{
		EXPECT_DOUBLE_EQ(polygon[i].first, expected[i].first);
		EXPECT_DOUBLE_EQ(polygon[i].second, expected[i].second);
	}
}

TEST(GeometryUtilsTest, IsPointInPolygon)
{
	// Прямоугольник
	Polygon rect = { { 0, 0 }, { 2, 0 }, { 2, 2 }, { 0, 2 } };

	// Точка внутри
	EXPECT_TRUE(GeometryUtils::IsPointInPolygon(rect, { 1, 1 }));
	// Точка снаружи
	EXPECT_FALSE(GeometryUtils::IsPointInPolygon(rect, { 3, 3 }));
	// Точка на границе
	EXPECT_FALSE(GeometryUtils::IsPointInPolygon(rect, { 0, 1 }));
	// Точка в вершине
	EXPECT_FALSE(GeometryUtils::IsPointInPolygon(rect, { 2, 2 }));

	// Сложный многоугольник (форма звезды)
	Polygon star = { { 0, 0 }, { 2, 2 }, { 4, 0 }, { 2, 1 }, { 0, 0 } };
	EXPECT_TRUE(GeometryUtils::IsPointInPolygon(star, { 1, 0.5 }));
	EXPECT_TRUE(GeometryUtils::IsPointInPolygon(star, { 2, 1.5 }));
}

TEST(GeometryUtilsTest, IsSimplePolygon)
{
	// Простой многоугольник (без самопересечений)
	Polygon simple = { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } };
	EXPECT_TRUE(GeometryUtils::IsSimplePolygon(simple));

	// Самопересекающийся многоугольник (форма бабочки)
	Polygon butterfly = { { 0, 0 }, { 2, 2 }, { 2, 0 }, { 0, 2 } };
	EXPECT_FALSE(GeometryUtils::IsSimplePolygon(butterfly));

	// Вырожденный случай (все точки на одной линии)
	Polygon degenerate = { { 0, 0 }, { 1, 1 }, { 2, 2 }, { 3, 3 } };
	EXPECT_TRUE(GeometryUtils::IsSimplePolygon(degenerate));
}

// TEST(GeometryUtilsTest, IsDiagonalValid)
// {
// 	// Квадрат
// 	Polygon square = { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } };

// 	// Валидная диагональ
// 	EXPECT_TRUE(GeometryUtils::IsDiagonalValid(square, 0, 2));
// 	// Невалидная (ребро)
// 	EXPECT_FALSE(GeometryUtils::IsDiagonalValid(square, 0, 1));
// 	// Невалидная (пересекает ребро)
// 	Polygon concave = { { 0, 0 }, { 2, 0 }, { 2, 1 }, { 1, 0.5 }, { 0, 1 } };
// 	EXPECT_FALSE(GeometryUtils::IsDiagonalValid(concave, 0, 3));
// }

TEST(GeometryUtilsTest, GetSegmentsIntersection)
{
	Point intersection;

	// Пересекающиеся отрезки
	EXPECT_TRUE(GeometryUtils::GetSegmentsIntersection(
		{ 0, 0 }, { 2, 2 }, { 0, 2 }, { 2, 0 }, intersection));
	EXPECT_DOUBLE_EQ(intersection.first, 1.0);
	EXPECT_DOUBLE_EQ(intersection.second, 1.0);

	// Непересекающиеся отрезки
	EXPECT_FALSE(GeometryUtils::GetSegmentsIntersection(
		{ 0, 0 }, { 1, 1 }, { 0, 2 }, { 1, 3 }, intersection));

	// Параллельные отрезки
	EXPECT_FALSE(GeometryUtils::GetSegmentsIntersection(
		{ 0, 0 }, { 1, 1 }, { 0, 1 }, { 1, 2 }, intersection));
}

TEST(GeometryUtilsTest, AreNeighbors)
{
	// Для многоугольника с 4 вершинами
	EXPECT_TRUE(GeometryUtils::AreNeighbors(0, 1, 4));
	EXPECT_TRUE(GeometryUtils::AreNeighbors(1, 0, 4));
	EXPECT_TRUE(GeometryUtils::AreNeighbors(3, 0, 4));
	EXPECT_TRUE(GeometryUtils::AreNeighbors(0, 3, 4));
	EXPECT_FALSE(GeometryUtils::AreNeighbors(0, 2, 4));
	EXPECT_FALSE(GeometryUtils::AreNeighbors(1, 3, 4));
}

TEST(GeometryUtilsTest, PointsAreEqualGeometrically)
{
	EXPECT_TRUE(GeometryUtils::PointsAreEqualGeometrically({ 1.0, 2.0 }, { 1.0, 2.0 }));
	EXPECT_TRUE(GeometryUtils::PointsAreEqualGeometrically(
		{ 1.0000000001, 2.0000000001 }, { 1.0, 2.0 }));
	EXPECT_FALSE(GeometryUtils::PointsAreEqualGeometrically({ 1.0, 2.0 }, { 1.1, 2.0 }));
	EXPECT_FALSE(GeometryUtils::PointsAreEqualGeometrically({ 1.0, 2.0 }, { 1.0, 2.1 }));
}

TEST(GeometryUtilsTest, SortPointsAlongEdge)
{
	std::vector<Point> points = { { 3, 3 }, { 1, 1 }, { 4, 4 }, { 2, 2 } };
	Point startPoint = { 0, 0 };
	std::vector<Point> expected = { { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };

	GeometryUtils::SortPointsAlongEdge(points, startPoint);

	ASSERT_EQ(points.size(), expected.size());
	for (size_t i = 0; i < expected.size(); ++i)
	{
		EXPECT_DOUBLE_EQ(points[i].first, expected[i].first);
		EXPECT_DOUBLE_EQ(points[i].second, expected[i].second);
	}
}