#ifndef GRAPH_H
#define GRAPH_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <set>
#include <utility>
#include <vector>

class Graph;

enum class state
{
	white, // Не посещенная
	gray,  // Посещенная (не завершенная)
	black  // Посещенная (завершенная)
};

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

// TODO: подумать как ведет себя вектор с size_t
using NumberList = std::vector<size_t>;

using SetList = std::set<NumberList>;

// Компоненты графа (малые графы)
using Components = std::vector<Graph>;

/// @brief Класс граф
class Graph
{

public:
	Graph(const AdjacencyMatrix& adjacencyMatrix, const Coordinates& coordinates);

	// Геттеры и сеттеры
	AdjacencyMatrix GetMatrix(void) const;
	Coordinates GetCoordinates(void) const;
	void SetMatrix(const AdjacencyMatrix& matrix);
	void SetCoordinates(const Coordinates& coordinates);

	void AddEdge(size_t from, size_t to);
	void RemoveEdge(size_t from, size_t to);
	Components GetViewComponents() const;
	SetList GetFunGraphs(const Components& viewComponents);
	bool AreEdgesInteract(const Edge& a, const Edge& b) const;

	// SFML отрисовка
	void Draw(sf::RenderWindow& window) const;

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
	ListEdge CollectEdges() const;
	NumberList FindViewComponents(const size_t i, const Components& viewComponents) const;
	AdjacencyMatrix GetViewMatrix(const ListEdge& graphEdges) const;
	CandidateList SortCandidates(const size_t i) const;
	void UpdateViewMatrix(const size_t i, const CandidateList& cands, const ListEdge& graphEdges, AdjacencyMatrix& viewMatrix) const;
	Components FindConnectedComponents(const AdjacencyMatrix& viewMatrix, const Coordinates& coordinates) const;
	NumberList DFS(size_t vertex, size_t numberVertices, const AdjacencyMatrix& viewMatrix, std::vector<state>& visited) const;
	Graph CreateComponent(const NumberList& componentVertices, const AdjacencyMatrix& viewMatrix, const Coordinates& coordinates) const;

private:
	AdjacencyMatrix m_matrix;
	Coordinates m_coordinates;
	sf::ConvexShape m_polygon;
};

/// @brief Класс адаптер
class FileToGraphAdapter
{
public:
	Graph ConvertEdgeListToMatrix(const std::string& fileName);

private:
	template <typename T>
	static T SafeRead(std::ifstream& file, const std::string& errorMessage);

	void AssertIsFileOpen(std::ifstream& file, const std::string& fileName) const;
	void AssertIsStreamCorrect(std::ifstream& file) const;
	void AssertIsValidSize(size_t size) const;
	void AssertIsValidNumbers(size_t from, size_t to, size_t matrixSize) const;
};

// Перегрузка оператора вывода из потока для графа
std::ostream& operator<<(std::ostream& os, const Graph& graph);

#endif // GRAPH_H