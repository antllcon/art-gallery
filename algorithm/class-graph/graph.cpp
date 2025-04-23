#include "graph.h"
#include "../config.h"
#include "../visibility/src/visilibity.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <numeric>
#include <stack>

Graph::Graph(const AdjacencyMatrix& adjacencyMatrix, const Coordinates& coordinates)
	: m_matrix(adjacencyMatrix)
	, m_coordinates(coordinates)
{
	AssertIsMatrixValid();
	AssertIsCoordinatesExist();
	AssertIsEqualSize();

	// Установка визуализации
	m_polygon.setFillColor(color::DARK);
	m_polygon.setOutlineColor(color::WHITE);
	m_polygon.setOutlineThickness(constants::THICKNESS);

	// Вычисление смещения
	std::tie(m_offsetX, m_offsetY) = CalculateGraphOffset();

	// Установка геометрии
	size_t numberVertices = coordinates.size();
	m_polygon.setPointCount(numberVertices);

	for (size_t i = 0; i < numberVertices; ++i)
	{
		m_polygon.setPoint(i, {static_cast<float>(m_coordinates[i].first) + m_offsetX, static_cast<float>(m_coordinates[i].second) + m_offsetY});
	}

	// Формат вершины
	m_vertex.setRadius(constants::RADIUS);
	m_vertex.setFillColor(color::GREEN);
	m_vertex.setOutlineColor(color::WHITE);
	m_vertex.setOutlineThickness(constants::THICKNESS);

	// Загрузка шрифта
	if (!m_font.loadFromFile(fonts::CASCADILIA_MONO))
	{
		throw std::runtime_error("Не удалось загрузить шрифт");
	}

	// Формат текста
	m_text.setFont(m_font);
	m_text.setCharacterSize(text::SIZE);
	m_text.setFillColor(color::BLACK);
}

AdjacencyMatrix Graph::GetMatrix(void) const
{
	return m_matrix;
}

Coordinates Graph::GetCoordinates(void) const
{
	return m_coordinates;
}

void Graph::AddEdge(size_t from, size_t to)
{
	AssertIsValidEdge(from, to);
	m_matrix[from][to] = m_matrix[to][from] = constants::EXIST;
}

void Graph::RemoveEdge(size_t from, size_t to)
{
	AssertIsValidEdge(from, to);
	m_matrix[from][to] = m_matrix[to][from] = constants::EMPTY;
}

void Graph::SetMatrix(const AdjacencyMatrix& matrix)
{
	m_matrix = matrix;
	AssertIsMatrixValid();
}

void Graph::SetCoordinates(const Coordinates& coordinates)
{
	m_coordinates = coordinates;
	AssertIsCoordinatesExist();
	AssertIsEqualSize();
}

// Получение компонент видимости
Components Graph::GetViewComponents() const
{
	Components components;

	// 1. Предобработка = соберем ребра исходного графа
	auto grpahEdges = CollectEdges();

	// 2. Построим полную матрицу видимости
	auto visibilityMatrix = GetViewMatrix(grpahEdges);

	std::cout << "\nМатрица видимости: " << '\n';

	for (const auto& row : visibilityMatrix)
	{
		for (int value : row)
		{
			std::cout << value << ' ';
		}
		std::cout << '\n';
	}

	// 3. Поиск связных компонент в графе видимости (DFS)
	// auto visibilityComponents = FindConnectedComponents(visibilityMatrix, m_coordinates);

	// return visibilityComponents;

	return components;
}

// Получение веерных графов
SetList Graph::GetFunGraphs(const Components& viewComponents)
{
	SetList funGraphs;
	size_t numberVertecies = m_coordinates.size();

	for (size_t i = 0; i < numberVertecies; ++i)
	{
		// Подумать должны ли компоненты видимости обрабатфватсья как графы
		// Собираем все компоненты видимости, которые видны из текущей вершины
		auto funGraph = FindViewComponents(i, viewComponents);
		funGraphs.insert(funGraph);
	}

	return funGraphs;
}

// TODO: Исправить логику - сгенерил код
NumberList Graph::FindViewComponents(const size_t i, const Components& viewComponents) const
{
	NumberList funGraph;

	// Реализуйте логику метода здесь
	// Например, перебор компонентов видимости и проверка их видимости из вершины i

	return funGraph;
}

// Проверка, пересекаются ли два отрезка
bool Graph::AreEdgesInteract(const Edge& a, const Edge& b) const
{
	// 1. Проверка ограничивающих прямоугольников
	if (!IsBoundBox(a, b))
	{
		return false;
	}

	// 2. Проверка взаимного расположения
	bool isCrossingFirst = ArePointsOpposite(a.first, a.second, b.first, b.second);
	bool isCrossingSecond = ArePointsOpposite(b.first, b.second, a.first, a.second);

	return isCrossingFirst && isCrossingSecond;
}

void Graph::Draw(sf::RenderWindow& window)
{
	window.draw(m_polygon);

	// Отрисовка рёбер
	// sf::VertexArray edges(sf::Lines);
	// for (size_t i = 0; i < m_matrix.size(); ++i)
	// {
	// 	for (size_t j = i + 1; j < m_matrix.size(); ++j)
	// 	{
	// 		if (m_matrix[i][j] == constants::EXIST)
	// 		{
	// 			// Добавляем вершины рёбер с учётом смещения
	// 			edges.append(sf::Vertex(
	// 				sf::Vector2f(static_cast<float>(m_coordinates[i].first) + m_offsetX,
	// 					static_cast<float>(m_coordinates[i].second) + m_offsetY),
	// 				color::WHITE));
	// 			edges.append(sf::Vertex(
	// 				sf::Vector2f(static_cast<float>(m_coordinates[j].first) + m_offsetX,
	// 					static_cast<float>(m_coordinates[j].second) + m_offsetY),
	// 				color::WHITE));
	// 		}
	// 	}
	// }
	// window.draw(edges);

	// Отрисовка вершин
	for (size_t i = 0; i < m_coordinates.size(); ++i)
	{
		const auto& vertex = m_coordinates[i];

		// Отрисовка кружка
		m_vertex.setPosition(static_cast<float>(vertex.first) + m_offsetX - m_vertex.getRadius(),
			static_cast<float>(vertex.second) + m_offsetY - m_vertex.getRadius());
		window.draw(m_vertex);

		// Отрисовка номера вершины
		m_text.setString(std::to_string(i + 1));
		m_text.setPosition(static_cast<float>(vertex.first) + m_offsetX - 4,
			static_cast<float>(vertex.second) + m_offsetY - 8);
		window.draw(m_text);
	}
}

void Graph::AssertIsEqualSize() const
{
	if (m_coordinates.size() != m_matrix.size())
	{
		throw std::runtime_error("Количества координат и вершин графа не совпадают");
	}
}

void Graph::AssertIsCoordinatesExist() const
{
	if (m_coordinates.empty())
	{
		throw std::runtime_error("Координаты пустые");
	}
}

void Graph::AssertIsMatrixValid() const
{
	AssertIsMatrixExist();
	AssertIsMatrixSquare();
}

void Graph::AssertIsMatrixExist() const
{
	if (m_matrix.empty())
	{
		throw std::runtime_error("Матрица пустая");
	}
}

void Graph::AssertIsMatrixSquare() const
{
	size_t size = m_matrix.size();
	for (const auto& row : m_matrix)
	{
		if (row.size() != size)
		{
			throw std::runtime_error("Матрица не квадратная");
		}
	}
}

void Graph::AssertIsValidEdge(size_t from, size_t to) const
{
	if (from >= m_matrix.size() || to >= m_matrix.size())
	{
		throw std::runtime_error("Неверный номер вершины");
	}
	if (from == to)
	{
		throw std::runtime_error("Номера вершин совпадают");
	}
}

// Проверка коллизии двух отрезков (их коробок)
bool Graph::IsBoundBox(const Edge& a, const Edge& b) const
{
	// Границы многоугольника A
	int aMinX = std::min(a.first.first, a.second.first);
	int aMaxX = std::max(a.first.first, a.second.first);
	int aMinY = std::min(a.first.second, a.second.second);
	int aMaxY = std::max(a.first.second, a.second.second);

	// Границы многоугольника B
	int bMinX = std::min(b.first.first, b.second.first);
	int bMaxX = std::max(b.first.first, b.second.first);
	int bMinY = std::min(b.first.second, b.second.second);
	int bMaxY = std::max(b.first.second, b.second.second);

	// Проверка на пересечение
	return (aMaxX >= bMinX) && (bMaxX >= aMinX) && (aMaxY >= bMinY) && (bMaxY >= aMinY);
}

// Проверка, что точки a и b находятся по разные стороны от прямой, проходящей через c и d
bool Graph::ArePointsOpposite(const Vertex& a, const Vertex& b, const Vertex& c, const Vertex& d) const
{
	// Вычисляем векторные произведения
	int crossFirst = CrossProduct(c, d, a);
	int crossSecond = CrossProduct(c, d, b);

	// Если произведение, то точки находятся по одну сторону
	return (crossSecond * crossFirst) < 0;
}

// Скалярное произведение вектора AB на вектор AC
int Graph::CrossProduct(const Vertex& a, const Vertex& b, const Vertex& c) const
{
	return (b.first - a.first) * (c.second - a.second) - (b.second - a.second) * (c.first - a.first);
}

// Собираем все ребра графа
ListEdge Graph::CollectEdges() const
{
	ListEdge edges;
	size_t numberVertices = m_coordinates.size();

	for (size_t i = 0; i < numberVertices; ++i)
	{
		for (size_t j = i + 1; j < numberVertices; ++j)
		{
			if (m_matrix[i][j] == constants::EXIST)
			{
				edges.emplace_back(Edge{m_coordinates[i], m_coordinates[j]});
			}
		}
	}

	return edges;
}

AdjacencyMatrix Graph::GetViewMatrix(const ListEdge& graphEdges) const
{
	size_t n = m_coordinates.size();
	AdjacencyMatrix viewMatrix(n, std::vector<size_t>(n, constants::EMPTY));

	GraphToEnvironment adapter;
	std::vector<Coordinates> holes = {};
	VisiLibity::Environment env = adapter.CreateEnvironment(m_coordinates, holes);

	VisiLibity::Visibility_Graph viewGraph(env, 1e-6);

	for (size_t i = 0; i < n; ++i)
	{
		for (size_t j = i + 1; j < n; ++j)
		{
			if (AreVerticesVisible(viewGraph, i, j))
			{
				viewMatrix[i][j] = viewMatrix[j][i] = constants::EXIST;
			}
		}
	}

	// std::cout << std::endl
	//		  << "== Вывод кандидатов для вершин ==" << std::endl;
	// for (size_t i = 0; i < n; ++i)
	// {
	// 	// В теории работает, подумать надо ли это вообще
	// 	// 2.1 Сортировка других вершин по возрастанию угла около текущей вершины
	// 	auto cands = SortCandidates(i);
	// 	size_t sizeCands = cands.size();

	// 	std::cout << "Для вершины №" << i + 1 << " = [";
	// 	for (size_t test = 0; test < sizeCands; ++test)
	// 	{
	// 		std::cout << cands[test].index + 1;
	// 		if (test != sizeCands - 1)
	// 		{
	// 			std::cout << ", ";
	// 		}
	// 	}
	// 	std::cout << "]" << std::endl;

	// 	// Вывод кандидатов текущей вершины

	// 	// 2.2 Обход кандидатов в порядке возрастания угла
	// 	// Здесь должна быть логика Case1/Case2 из Вельцла
	// 	UpdateViewMatrix(i, cands, graphEdges, viewMatrix);
	// }
	return viewMatrix;
}

// Сортируем кандидатов по углу относительно текущей вершины
CandidateList Graph::SortCandidates(const size_t i) const
{
	CandidateList candidates;
	size_t n = m_coordinates.size();
	candidates.reserve(n - 1);
	const Vertex& vi = m_coordinates[i];

	for (size_t j = 0; j < n; ++j)
	{
		if (i == j)
		{
			continue;
		}

		const Vertex& vj = m_coordinates[j];
		double angle = std::atan2(vj.second - vi.second, vj.first - vi.first);

		// Нормализуем угол в диапазон [0, 2π]
		double normalizedAngle = (angle < 0) ? angle + 2 * M_PI : angle;

		candidates.push_back({j, angle});
	}

	std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
		return a.angle < b.angle;
	});

	return candidates;
}

// Обновление матрицы видимости
// Здесь должна быть логика Case1/Case2 из Вельцла
void Graph::UpdateViewMatrix(const size_t i, const CandidateList& cands, const ListEdge& graphEdges, AdjacencyMatrix& viewMatrix) const
{
	const Vertex vi = m_coordinates[i];

	for (auto& cand : cands)
	{
		size_t j = cand.index;
		Edge sk = {vi, m_coordinates[j]};

		// Здесь вместо полного перебора ребер можно было бы:
		// Case1: проверить, пересекает ли ab текущий блокирующий сегмент sk (O(1)).
		// Если пересекает — невидим, continue.
		// Case2: иначе — ab видим, обновляем sk/ ui по подслучаям 2a/2b/2c.

		// В данной упрощённой версии делаем наивную проверку пересечений:
		bool visible = true;
		for (auto& edge : graphEdges)
		{
			if (sk == edge)
				continue;

			if (AreEdgesInteract(sk, edge))
			{
				visible = false;
				break;
				// Case1 «наивно»
			}
		}

		if (visible)
		{
			// Case2 «наивно»
			viewMatrix[i][j] = viewMatrix[j][i] = constants::EXIST;
		}
	};
}

// Вспомогательная функция для поиска связных компонент графа видимости
Components Graph::FindConnectedComponents(const AdjacencyMatrix& viewMatrix, const Coordinates& coordinates) const
{
	Components components;
	const size_t numberVertices = viewMatrix.size();
	std::vector<state> visited(numberVertices, state::white);

	for (size_t v = 0; v < numberVertices; ++v)
	{
		if (visited[v] != state::white)
		{
			continue;
		}

		auto componentVertices = DFS(v, numberVertices, viewMatrix, visited);
		auto component = CreateComponent(componentVertices, viewMatrix, coordinates);
		components.push_back(component);
	}

	return components;
}

// DFS для поиска всех вершин, достижимых из заданной
NumberList Graph::DFS(size_t vertex, size_t numberVertices, const AdjacencyMatrix& viewMatrix, std::vector<state>& visited) const
{
	std::stack<size_t> stackBuffer;
	stackBuffer.push(vertex);
	visited[vertex] = state::gray;
	NumberList componentVertices;

	while (!stackBuffer.empty())
	{
		size_t currentVertex = stackBuffer.top();
		stackBuffer.pop();
		componentVertices.push_back(currentVertex);

		for (size_t neighbor = 0; neighbor < numberVertices; ++neighbor)
		{
			if (viewMatrix[currentVertex][neighbor] == constants::EXIST && visited[neighbor] == state::white)
			{
				stackBuffer.push(neighbor);
				visited[neighbor] = state::gray;
			}
		}

		visited[currentVertex] = state::black;
	}

	return componentVertices;
}

// Создаёт Graph из списка номеров вершин компоненты
Graph Graph::CreateComponent(const NumberList& componentVertices, const AdjacencyMatrix& viewMatrix, const Coordinates& coordinates) const
{
	Coordinates componentCoordinates;
	AdjacencyMatrix componentAdjacencyMatrix(componentVertices.size());

	for (size_t i = 0; i < componentVertices.size(); ++i)
	{
		size_t originalNodeI = componentVertices[i];
		componentCoordinates.push_back(coordinates[originalNodeI]);
		for (size_t j = i + 1; j < componentVertices.size(); ++j)
		{
			size_t originalNodeJ = componentVertices[j];
			if (viewMatrix[originalNodeI][originalNodeJ] == constants::EXIST)
			{
				componentAdjacencyMatrix[i][j] = componentAdjacencyMatrix[j][i] = constants::EXIST;
			}
		}
	}
	return Graph(componentAdjacencyMatrix, componentCoordinates);
}

std::pair<float, float> Graph::CalculateGraphOffset() const
{
	// Вычисление границ графа
	int minX = m_coordinates[0].first, maxX = m_coordinates[0].first;
	int minY = m_coordinates[0].second, maxY = m_coordinates[0].second;

	for (const auto& vertex : m_coordinates)
	{
		minX = std::min(minX, vertex.first);
		maxX = std::max(maxX, vertex.first);
		minY = std::min(minY, vertex.second);
		maxY = std::max(maxY, vertex.second);
	}

	// Центр графа
	float centerX = (minX + maxX) / 2.0f;
	float centerY = (minY + maxY) / 2.0f;

	// Центр окна
	float windowCenterX = screen::WIDTH / 2.0f;
	float windowCenterY = screen::HEIGHT / 2.0f;

	// Смещение для центрирования графа
	float offsetX = windowCenterX - centerX;
	float offsetY = windowCenterY - centerY;

	return {offsetX, offsetY};
}

bool Graph::AreVerticesVisible(const VisiLibity::Visibility_Graph& viewGraph, const size_t i, const size_t j) const
{
	return viewGraph(i, j);
}

Graph FileToGraphAdapter::ConvertEdgeListToMatrix(const std::string& fileName)
{
	std::ifstream file(fileName);
	AssertIsFileOpen(file, fileName);
	AssertIsStreamCorrect(file);

	size_t matrixSize = SafeRead<size_t>(file, "Некорректное чтение аргумента (размер матрицы)");
	AssertIsValidSize(matrixSize);

	AdjacencyMatrix matrix(matrixSize, std::vector<size_t>(matrixSize, constants::EMPTY));

	size_t from, to;
	std::string line;
	for (size_t i = 0; i < matrixSize; ++i)
	{
		file >> from >> line >> to;
		from--;
		to--;
		AssertIsValidNumbers(from, to, matrixSize);
		matrix[from][to] = matrix[to][from] = constants::EXIST;
	}

	AssertIsStreamCorrect(file);

	Coordinates coordinates(matrixSize);
	size_t x, y;
	for (size_t i = 0; i < matrixSize; ++i)
	{
		file >> x >> y;
		coordinates[i] = {x, y};
	}

	AssertIsStreamCorrect(file);
	Graph graph(matrix, coordinates);

	return graph;
}

template <typename T>
T FileToGraphAdapter::SafeRead(std::ifstream& file, const std::string& errorMessage)
{
	T value;
	if (!(file >> value))
	{
		throw std::runtime_error(errorMessage);
	}
	return value;
}

void FileToGraphAdapter::AssertIsFileOpen(std::ifstream& file, const std::string& fileName) const
{
	if (!file.is_open())
	{
		throw std::runtime_error("Невозможно открыть файл: " + fileName);
	}
}

void FileToGraphAdapter::AssertIsStreamCorrect(std::ifstream& file) const
{
	if (file.fail() && !file.eof())
	{
		throw std::runtime_error("Ошибка чтения данных из файла");
	}
}

void FileToGraphAdapter::AssertIsValidSize(size_t size) const
{
	if (size <= 0)
	{
		throw std::runtime_error("Некорректный размер матрицы");
	}
}

void FileToGraphAdapter::AssertIsValidNumbers(size_t from, size_t to, size_t matrixSize) const
{
	if (from >= matrixSize || to >= matrixSize)
	{
		throw std::runtime_error("Некорректные номера вершин");
	}
}

VisiLibity::Environment GraphToEnvironment::CreateEnvironment(const Coordinates& vertices, const std::vector<Coordinates>& holes)
{
	// Внешний контур (оболочка)
	std::vector<VisiLibity::Point> outerPoints;
	for (const auto& [x, y] : vertices)
	{
		outerPoints.emplace_back(static_cast<double>(x), static_cast<double>(y));
	}
	VisiLibity::Polygon outerBoundary(outerPoints);

	// Отверстия
	std::vector<VisiLibity::Polygon> holePolygons;
	for (const auto& hole : holes)
	{
		std::vector<VisiLibity::Point> holePoints;
		for (const auto& [x, y] : hole)
		{
			holePoints.emplace_back(static_cast<double>(x), static_cast<double>(y));
		}
		holePolygons.emplace_back(holePoints);
	}

	// Создание окружения
	std::vector<VisiLibity::Polygon> polygons;
	polygons.push_back(outerBoundary);										   // Внешний контур
	polygons.insert(polygons.end(), holePolygons.begin(), holePolygons.end()); // Отверстия

	return VisiLibity::Environment(polygons);
}

std::ostream& operator<<(std::ostream& os, const Graph& graph)
{
	os << "Матрица смежности галереи: " << '\n';

	for (const auto& row : graph.GetMatrix())
	{
		for (int value : row)
		{
			os << value << ' ';
		}
		os << '\n';
	}

	os << '\n';
	os << "Координаты точек: " << '\n';

	for (const auto& coords : graph.GetCoordinates())
	{

		os << "{ " << coords.first << "; " << coords.second << " }";
		os << '\n';
	}

	return os;
}