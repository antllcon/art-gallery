#ifndef GRAPH_H
#define GRAPH_H

#include <algorithm>
#include <numeric>
#include <unordered_map>

#include "../visibility/src/visilibity.hpp"
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/mark_domain_in_triangulation.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <set>
#include <utility>
#include <vector>

// CGAL Kernel и триангуляция с ограничениями
using K = CGAL::Exact_predicates_inexact_constructions_kernel;
using CDT = CGAL::Constrained_Delaunay_triangulation_2<K>;
using Point = K::Point_2;

class GraphUtils;
class Graph;

enum class state
{
	white, // Не посещенная
	gray,  // Посещенная (не завершенная)
	black  // Посещенная (завершенная)
};

using VisitState = std::vector<state>;

// Кандидат для сортировки
struct Candidate
{
	size_t index; // Индекс вершины
	double angle; // Угол
};

// Список кандидатов
using CandidateList = std::vector<Candidate>;
// Вершины графа
using Vertex = std::pair<int, int>;
// Ребро (связь между вершинами)
using Edge = std::pair<Vertex, Vertex>;
// Список ребер
using ListEdge = std::vector<Edge>;
// Координаты вершин
using Coordinates = std::vector<Vertex>;
// Матрица смежности
using AdjacencyMatrix = std::vector<std::vector<size_t>>;
// Охранники
using Guards = std::vector<std::pair<int, int>>;
// Список номеров
using NumberList = std::vector<size_t>;
// Список оригинальных списков
using SetList = std::set<NumberList>;
// Компоненты графа (малые графы)
using Components = std::vector<Graph>;
// Отверстия
using Holes = std::vector<Coordinates>;

// Класс граф
class Graph
{
public:
	Graph(const AdjacencyMatrix& matrix, const Coordinates& coordinates, const Holes& holes);

	// Геттеры и сеттеры
	AdjacencyMatrix GetMatrix(void) const;
	Coordinates GetCoordinates(void) const;
	Holes GetHoles(void) const;
	void SetMatrix(const AdjacencyMatrix& matrix);
	void SetCoordinates(const Coordinates& coordinates);
	void SetHoles(const Holes& holes);
	AdjacencyMatrix GetViewMatrix() const;

	// Методы обработки графа
	void AddEdge(size_t from, size_t to);
	void RemoveEdge(size_t from, size_t to);
	Components GetViewComponents() const;
	SetList GetFunGraphs(const Components& viewComponents);
	bool AreEdgesInteract(const Edge& a, const Edge& b) const;
	bool AreVerticesVisible(const VisiLibity::Visibility_Graph& viewGraph, const size_t i, const size_t j) const;

	// SFML отрисовка
	void Draw(sf::RenderWindow& window);

private:
	void AssertIsEqualSize() const;
	void AssertIsCoordinatesExist() const;
	void AssertIsMatrixValid() const;
	void AssertIsMatrixExist() const;
	void AssertIsMatrixSquare() const;
	void AssertIsValidEdge(size_t from, size_t to) const;
	bool IsBoundBox(const Edge& a, const Edge& b) const;
	bool ArePointsOpposite(const Vertex& a, const Vertex& b, const Vertex& c, const Vertex& d) const;
	int CrossProduct(const Vertex& a, const Vertex& b, const Vertex& c) const;
	NumberList FindViewComponents(const size_t i, const Components& viewComponents) const;
	CandidateList SortCandidates(const size_t i) const;
	void UpdateViewMatrix(const size_t i, const CandidateList& cands, const ListEdge& graphEdges, AdjacencyMatrix& viewMatrix) const;
	// Components FindConnectedComponents(const AdjacencyMatrix& viewMatrix, const Coordinates& coordinates) const;
	// Graph CreateComponent(const NumberList& componentVertices, const AdjacencyMatrix& viewMatrix, const Coordinates& coordinates) const;
	std::pair<float, float> CalculateGraphOffset() const;
	Coordinates DetectPolygon(const AdjacencyMatrix& matrix, const Coordinates& coordinates) const;

	CDT PlaneSweepTriangulation() const;
	AdjacencyMatrix BuildView(const CDT& cdt) const;

private:
	AdjacencyMatrix m_matrix;
	Coordinates m_coordinates;
	Holes m_holes;

	CDT m_cdt;
	std::vector<CDT::Vertex_handle> m_vh;

	// SFML
	sf::ConvexShape m_polygon;
	sf::CircleShape m_vertex;
	sf::Font m_font;
	sf::Text m_text;
	float m_offsetX;
	float m_offsetY;
};

/// @brief Класс адаптер
class FileToGraphAdapter
{
public:
	Graph ConvertEdgeListToMatrix(const std::string& fileName);

private:
	AdjacencyMatrix ReadAdjacencyMatrix(std::ifstream& file, size_t matrixSize) const;
	Coordinates ReadCoordinates(std::ifstream& file, size_t matrixSize) const;
	Holes DetectHoles(const AdjacencyMatrix& matrix, const Coordinates& coordinates) const;

	template <typename T>
	static T SafeRead(std::ifstream& file, const std::string& errorMessage);

	void AssertIsFileOpen(std::ifstream& file, const std::string& fileName) const;
	void AssertIsStreamCorrect(std::ifstream& file) const;
	void AssertIsValidSize(size_t size) const;
	void AssertIsValidNumbers(size_t from, size_t to, size_t matrixSize) const;
};

class GraphToEnvironment
{
public:
	VisiLibity::Environment CreateEnvironment(const Coordinates& vertices, const Holes& holes);
};

class GraphUtils
{
public:
	static NumberList DFS(size_t vertex, size_t numberVertices, const AdjacencyMatrix& viewMatrix, std::vector<state>& visited);
	static bool IsCycle(const NumberList& vertices, const AdjacencyMatrix& matrix);
	static bool IsClockwise(const Coordinates& polygon);
	static void EnsureCounterClockwise(Coordinates& polygon);
	static void EnsureClockwise(Coordinates& polygon);
	static size_t FindIndex(const Coordinates& C, const Point& p);
};

// Перегрузка оператора вывода из потока для графа
std::ostream& operator<<(std::ostream& os, const Graph& graph);
std::ostream& operator<<(std::ostream& os, const Holes& holes);

#endif // GRAPH_H