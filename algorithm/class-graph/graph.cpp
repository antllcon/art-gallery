#include "graph.h"
#include <algorithm>
#include <fstream>
#include <numeric>

TimeMatrix Graph::CreateTimeCopy() const { return TimeMatrix(vertexCount); }

Component Graph::ReverseSortExitTime(TimeMatrix& matrix) const
{
	Component vertexes(vertexCount);
	std::iota(vertexes.begin(), vertexes.end(), 0);
	std::sort(vertexes.begin(), vertexes.end(), [&](int a, int b) { return matrix[a].exitTime > matrix[b].exitTime; });
	return vertexes;
}

Graph::Graph(int numberVertices, bool isDirected)
	: vertexCount(numberVertices)
	, matrix(numberVertices, std::vector<int>(numberVertices, 0))
	, isDirected(isDirected)
{
	if (numberVertices <= 0)
	{
		throw std::invalid_argument("Неверное число узлов");
	}
}

Graph::Graph(const AdjacencyMatrix& adjacencyMatrix, bool isDirected)
	: matrix(adjacencyMatrix)
	, vertexCount(adjacencyMatrix.size())
	, isDirected(isDirected)
{
	if (vertexCount == 0)
	{
		throw std::invalid_argument("Матрица смежности не может быть пустой");
	}

	for (const auto& row : adjacencyMatrix)
	{
		if (row.size() != vertexCount)
		{
			throw std::invalid_argument("Матрица смежности должна быть квадратной");
		}
	}
}

void Graph::AssertIsCoordsInRange(int x, int y) const
{
	if (x >= vertexCount || y >= vertexCount || x < 0 || y < 0)
	{
		throw std::runtime_error("Incorrect cord. of matrix size");
	}
}

void Graph::AddEdge(int from, int to, int value)
{
	from = from - 1;
	to = to - 1;
	AssertIsCoordsInRange(from, to);
	matrix[from][to] = value;
	if (!isDirected)
	{
		matrix[from][to] = value;
	}
}

void Graph::RemoveEdge(int from, int to)
{
	from = from - 1;
	to = to - 1;
	AssertIsCoordsInRange(from, to);
	matrix[from][to] = 0;
	if (!isDirected)
	{
		matrix[from][to] = 0;
	}
}

const AdjacencyMatrix& Graph::GetMatrix(void) const { return matrix; }

Graph Graph::GetInvertGraph() const
{
	Graph transposedGraph(vertexCount);
	transposedGraph.matrix = TranspositionMatrix(this->matrix);
	return transposedGraph;
}

AdjacencyMatrix Graph::TranspositionMatrix(const AdjacencyMatrix& matrix) const
{
	int size = matrix.size();
	AdjacencyMatrix transposed(size, std::vector<int>(size, 0));

	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			transposed[j][i] = matrix[i][j];
		}
	}
	return transposed;
}

void Graph::DFS(int i, const AdjacencyMatrix& matrix, TimeMatrix& timeMatrix, int& currentTime, Component* component = nullptr)
{
	timeMatrix[i].visited = color::gray;
	timeMatrix[i].entryTime = currentTime++;

	// Перегрузка функции
	if (component)
	{
		component->push_back(i);
	}

	for (int j = 0; j < matrix.size(); ++j)
	{
		if (matrix[i][j] != 0 && timeMatrix[j].visited == color::white)
		{
			DFS(j, matrix, timeMatrix, currentTime, component);
		}
	}

	timeMatrix[i].exitTime = currentTime++;
	timeMatrix[i].visited = color::black;
}

TimeMatrix Graph::DFSWithTimestamps()
{
	TimeMatrix matrix = CreateTimeCopy();
	int time = 0;

	for (size_t i = 0; i < vertexCount; ++i)
	{
		if (matrix[i].visited == color::white)
		{
			DFS(i, this->matrix, matrix, time);
		}
	}

	return matrix;
}

Components Graph::GetStrongComponentsKosaraju()
{
	// Запуск похода в глубину и сортировкой всех вершин в
	// порядке убывания времени выхода
	TimeMatrix matrix = DFSWithTimestamps();
	Component sortedVertexes = ReverseSortExitTime(matrix);

	// Строим обратный граф (транспонируем матрицу смежности)
	AdjacencyMatrix transpositionMatrix = TranspositionMatrix(this->matrix);

	// Запуск обхода в глубину в обратном графе
	// (в порядке убывания вершин)
	Components components = DFSWithCollectComponents(sortedVertexes, transpositionMatrix);

	return components;
}

Components Graph::DFSWithCollectComponents(const Component& sortedVertexes, AdjacencyMatrix& transMatrix)
{
	Components strongComponents;
	TimeMatrix matrix = CreateTimeCopy();
	int time = 0;

	for (int vertex : sortedVertexes)
	{
		if (matrix[vertex].visited == color::white)
		{
			Component component;
			DFS(vertex, transMatrix, matrix, time, &component);
			strongComponents.push_back(component);
		}
	}
	return strongComponents;
}

std::ostream& operator<<(std::ostream& os, const Graph& graph)
{
	for (const auto& row : graph.GetMatrix())
	{
		for (int value : row)
		{
			os << value << ' ';
		}
		os << '\n';
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const Components& components)
{
	for (const auto& component : components)
	{
		os << "[ ";
		for (int value : component)
		{
			os << value + 1 << ' ';
		}
		os << "]\n";
	}
	return os;
}

ListEdge FileToGraphAdapter::ConvertEdgeListToMatrix(const Graph& graph)
{
	ListEdge listEdge;
	const auto& matrix = graph.GetMatrix();
	int vertexCount = matrix.size();

	for (int i = 0; i < vertexCount; ++i)
	{
		for (int j = i; j < vertexCount; ++j)
		{
			if (matrix[i][j] != 0)
			{
				listEdge.push_back({i, j, matrix[i][j]});
			}
		}
	}

	return listEdge;
}

Graph FileToGraphAdapter::ConvertMatrixToEdgeList(const std::string& fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		throw std::runtime_error("Incorrect name of file" + fileName);
	}

	int matrixSize;
	if (!(file >> matrixSize) || matrixSize <= 0)
	{
		throw std::runtime_error("Invalid or missing matrix size in file: " + fileName);
	}

	Graph graph(matrixSize);

	int x, y, value;
	while (file >> x >> y >> value)
	{
		graph.AddEdge(x, y, value);
	}

	if (file.fail() && !file.eof())
	{
		throw std::runtime_error("Error while reading file: " + fileName);
	}

	return graph;
}