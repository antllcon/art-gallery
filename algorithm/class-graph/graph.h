#include <iostream>
#include <stack>
#include <vector>

enum class color
{
	white, // Не посещенная
	gray,  // Посещенная (не завершенная)
	black  // Посещенная (завершенная)
};

// Ребро стоиомсти
struct Edge
{
	int from;  // Из какого узла
	int to;	   // В какой узел
	int value; // Стоимость пути
};

// Временной узел
struct TimeNode
{
	color visited = color::white; // Состояние посещенности
	int entryTime = -1;			  // Время посещения вершины
	int exitTime = -1;			  // Время выхода из вершины
};

// Матрица смежности
using AdjacencyMatrix = std::vector<std::vector<int>>;

// Матрица времени
using TimeMatrix = std::vector<TimeNode>;

// Компонента связности (номера узлов)
using Component = std::vector<int>;

// Компоненты связности (массив компонент)
using Components = std::vector<Component>;

// Список ребер
using ListEdge = std::vector<Edge>;

/// @brief Класс граф
class Graph
{
	AdjacencyMatrix matrix;
	int vertexCount;
	bool isDirected;

	TimeMatrix CreateTimeCopy() const;
	void AssertIsCoordsInRange(int x, int y) const;
	Component ReverseSortExitTime(TimeMatrix& matrix) const;
	AdjacencyMatrix TranspositionMatrix(const AdjacencyMatrix& matrix) const;
	void DFS(int i, const AdjacencyMatrix& matrix, TimeMatrix& TimeMatrix, int& currentTime, Component* component);
	Components DFSWithCollectComponents(const Component& sortedVertexes, AdjacencyMatrix& transpositionMatrix);

public:
	Graph(int numberVertices, bool isDirected = false);
	Graph(const AdjacencyMatrix& adjacencyMatrix, bool isDirected = false);

	void AddEdge(int from, int to, int value = 1);
	void RemoveEdge(int from, int to);
	const AdjacencyMatrix& GetMatrix(void) const;
	Graph GetInvertGraph() const;
	TimeMatrix DFSWithTimestamps();
	Components GetStrongComponentsKosaraju();
};

/// @brief Класс адаптер
class FileToGraphAdapter
{
	static ListEdge ConvertEdgeListToMatrix(const Graph& graph);
	static Graph ConvertMatrixToEdgeList(const std::string& fileName);
};

// Перегрузка оператора вывода из потока для графа
std::ostream& operator<<(std::ostream& os, const Graph& graph);

// Перегрузка оператора вывода из потока для компонент связности
std::ostream& operator<<(std::ostream& os, const Components& components);