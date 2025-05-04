#include "graph.h"
#include "../config.h"
#include "../visibility/src/visilibity.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <numeric>
#include <stack>

Graph::Graph(const AdjacencyMatrix& matrix, const Coordinates& coordinates, const Holes& holes)
	: m_matrix(matrix)
	, m_coordinates(coordinates)
	, m_holes(holes)
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

Holes Graph::GetHoles(void) const
{
	return m_holes;
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

void Graph::SetHoles(const Holes& holes)
{
	// TODO: добавить проверки
	m_holes = holes;
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

// Получение компонент видимости
Components Graph::GetViewComponents() const
{
	Components components;

	// 1. Построим полную матрицу видимости
	auto visibilityMatrix = GetViewMatrix();

	// 2. Поиск связных компонент в графе видимости (DFS)
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
		// Подумать должны ли компоненты видимости обрабатываться как графы
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
	// window.draw(m_polygon);

	// Отрисовка рёбер
	sf::VertexArray edges(sf::Lines);
	for (size_t i = 0; i < m_matrix.size(); ++i)
	{
		for (size_t j = i + 1; j < m_matrix.size(); ++j)
		{
			if (m_matrix[i][j] == constants::EXIST)
			{
				// Добавляем вершины рёбер с учётом смещения
				edges.append(sf::Vertex(
					sf::Vector2f(static_cast<float>(m_coordinates[i].first) + m_offsetX,
						static_cast<float>(m_coordinates[i].second) + m_offsetY),
					color::WHITE));
				edges.append(sf::Vertex(
					sf::Vector2f(static_cast<float>(m_coordinates[j].first) + m_offsetX,
						static_cast<float>(m_coordinates[j].second) + m_offsetY),
					color::WHITE));
			}
		}
	}
	window.draw(edges);

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
	for (const auto& row : m_matrix)
	{
		if (row.size() != m_matrix.size())
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
	if (m_matrix[from][to] == constants::EXIST)
	{
		throw std::runtime_error("Ребро уже существует");
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

// Получить матрицу видимости
AdjacencyMatrix Graph::GetViewMatrix() const
{
	// 1) Выполнить триангуляцию свободного пространства
	auto cdt = PlaneSweepTriangulation();

	// 2) Построить видимость
	return BuildView(cdt);
}

CDT Graph::PlaneSweepTriangulation() const
{
	CDT cdt;

	std::vector<CDT::Vertex_handle> handles;
	for (const auto& coord : m_coordinates)
	{
		handles.push_back(cdt.insert(Point(coord.first, coord.second)));
	}

	// Добавляем ограничения (ребра), которые нельзя нарушать
	for (size_t i = 0; i < handles.size(); ++i)
	{
		cdt.insert_constraint(handles[i], handles[(i + 1) % handles.size()]);
	}

	// Проверяем, что триангуляция не пуста
	if (cdt.number_of_vertices() == 0)
	{
		throw std::runtime_error("Триангуляция не пустая");
	}

	return cdt;
}

AdjacencyMatrix Graph::BuildView(const CDT& cdt) const
{
	size_t n = m_coordinates.size();
	AdjacencyMatrix view(n, std::vector<size_t>(n, constants::EMPTY));

	if (cdt.number_of_vertices() == 0)
	{
		return view;
	}

	// Дальше не понятно
	auto get_index = [&](const Point& p) -> size_t {
		for (size_t i = 0; i < m_coordinates.size(); ++i)
		{
			if (std::abs(p.x() - m_coordinates[i].first) < constants::EPSILON && std::abs(p.y() - m_coordinates[i].second) < 1e-6)
			{
				return i;
			}
		}
		throw std::runtime_error("Point not found in coordinates");
	};

	for (auto eit = cdt.finite_edges_begin(); eit != cdt.finite_edges_end(); ++eit)
	{
		auto edge = *eit;
		auto face = edge.first;
		int idx = edge.second;

		auto vh1 = face->vertex(cdt.cw(idx));
		auto vh2 = face->vertex(cdt.ccw(idx));

		size_t i = get_index(vh1->point());
		size_t j = get_index(vh2->point());
		view[i][j] = constants::EXIST;
		view[j][i] = constants::EXIST;
	}
	return view;
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
// Components Graph::FindConnectedComponents(const AdjacencyMatrix& viewMatrix, const Coordinates& coordinates) const
// {
// 	Components components;
// 	const size_t numberVertices = viewMatrix.size();
// 	VisitState visited(numberVertices, state::white);

// 	for (size_t v = 0; v < numberVertices; ++v)
// 	{
// 		if (visited[v] != state::white)
// 		{
// 			continue;
// 		}

// 		// auto componentVertices = DFS(v, numberVertices, viewMatrix, visited);
// 		auto component = CreateComponent(componentVertices, viewMatrix, coordinates);
// 		components.push_back(component);
// 	}

// 	return components;
// }

// Создаёт Graph из списка номеров вершин компоненты
// Graph Graph::CreateComponent(const NumberList& componentVertices, const AdjacencyMatrix& viewMatrix, const Coordinates& coordinates) const
// {
// 	Coordinates componentCoordinates;
// 	AdjacencyMatrix componentAdjacencyMatrix(componentVertices.size(), std::vector<size_t>(componentVertices.size(), constants::EMPTY));

// 	for (size_t i = 0; i < componentVertices.size(); ++i)
// 	{
// 		size_t originalNodeI = componentVertices[i];
// 		componentCoordinates.push_back(coordinates[originalNodeI]);
// 		for (size_t j = i + 1; j < componentVertices.size(); ++j)
// 		{
// 			size_t originalNodeJ = componentVertices[j];
// 			if (viewMatrix[originalNodeI][originalNodeJ] == constants::EXIST)
// 			{
// 				componentAdjacencyMatrix[i][j] = componentAdjacencyMatrix[j][i] = constants::EXIST;
// 			}
// 		}
// 	}
// 	return Graph(componentAdjacencyMatrix, componentCoordinates, m_holes);
// }

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

	AdjacencyMatrix matrix = ReadAdjacencyMatrix(file, matrixSize);
	Coordinates coordinates = ReadCoordinates(file, matrixSize);
	Holes holes = DetectHoles(matrix, coordinates);

	Graph graph(matrix, coordinates, holes);
	return graph;
}

AdjacencyMatrix FileToGraphAdapter::ReadAdjacencyMatrix(std::ifstream& file, size_t matrixSize) const
{
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
	return matrix;
}

Coordinates FileToGraphAdapter::ReadCoordinates(std::ifstream& file, size_t matrixSize) const
{
	Coordinates coordinates(matrixSize);
	size_t x, y;
	for (size_t i = 0; i < matrixSize; ++i)
	{
		file >> x >> y;
		coordinates[i] = {x, y};
	}

	AssertIsStreamCorrect(file);
	return coordinates;
}

Holes FileToGraphAdapter::DetectHoles(const AdjacencyMatrix& matrix, const Coordinates& coordinates) const
{
	Holes holes;
	size_t numberVertices = coordinates.size();
	VisitState visited(numberVertices, state::white);

	for (size_t v = 0; v < numberVertices; ++v)
	{
		if (visited[v] != state::white)
		{
			continue;
		}

		// Используем DFS для поиска компонент
		NumberList result = GraphUtils::DFS(v, numberVertices, matrix, visited);

		// Проверяем, является ли компонент циклом
		if (!GraphUtils::IsCycle(result, matrix))
		{
			continue;
		}

		// Пропускаем цикл, если он содержит вершину 0 (главный цикл)
		if (std::find(result.begin(), result.end(), 0) != result.end())
		{
			continue;
		}

		// Преобразуем номера вершин в координаты
		Coordinates hole;
		for (size_t vertex : result)
		{
			hole.push_back(coordinates[vertex]);
		}

		// Разворот по часовой стрелке
		GraphUtils::EnsureClockwise(hole);

		holes.push_back(hole);
	}

	return holes;
}

Coordinates Graph::DetectPolygon(const AdjacencyMatrix& matrix, const Coordinates& coordinates) const
{
	Coordinates polygon;
	size_t numberVertices = coordinates.size();
	VisitState visited(numberVertices, state::white);

	for (size_t v = 0; v < numberVertices; ++v)
	{
		if (visited[v] != state::white)
		{
			continue;
		}

		// Используем DFS для поиска компонент
		NumberList result = GraphUtils::DFS(v, numberVertices, matrix, visited);

		// Проверяем, является ли компонент циклом
		if (!GraphUtils::IsCycle(result, matrix))
		{
			continue;
		}

		// Пропускаем цикл, если он содержит вершину 0 (главный цикл)
		if (std::find(result.begin(), result.end(), 0) == result.end())
		{
			continue;
		}

		// Преобразуем номера вершин в координаты
		for (size_t vertex : result)
		{
			polygon.push_back(coordinates[vertex]);
		}

		// Разворот против часовой стрелки
		GraphUtils::EnsureCounterClockwise(polygon);
	}

	return polygon;
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

VisiLibity::Environment GraphToEnvironment::CreateEnvironment(const Coordinates& vertices, const Holes& holes)
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

std::ostream& operator<<(std::ostream& os, const Holes& holes)
{
	os << "Отверстия в галерее: " << '\n';
	size_t i = 0;

	for (const auto& hole : holes)
	{
		std::cout << ++i << " - ";
		for (const auto& vertex : hole)
		{
			os << "(" << vertex.first << ", " << vertex.second << ") ";
		}
		os << '\n';
	}

	return os;
}

NumberList GraphUtils::DFS(size_t vertex, size_t numberVertices, const AdjacencyMatrix& viewMatrix, std::vector<state>& visited)
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
bool GraphUtils::IsCycle(const NumberList& vertices, const AdjacencyMatrix& matrix)
{
	if (vertices.size() < 3)
	{
		return false;
	}

	size_t first = vertices.front();
	size_t last = vertices.back();
	return matrix[first][last] == constants::EXIST;
}

// Проверка ориентации многоугольника
bool GraphUtils::IsClockwise(const Coordinates& polygon)
{
	double sum = 0.0;

	for (size_t i = 0; i < polygon.size(); ++i)
	{
		const auto& [x1, y1] = polygon[i];
		const auto& [x2, y2] = polygon[(i + 1) % polygon.size()];
		sum += (x2 - x1) * (y2 + y1);
	}

	// Положительное значение — по часовой стрелке
	return sum > 0;
}

// Разворот многоугольника против часовой стрелки
void GraphUtils::EnsureCounterClockwise(Coordinates& polygon)
{
	if (IsClockwise(polygon))
	{
		std::reverse(polygon.begin(), polygon.end());
	}
}

// Разворот многоугольника по часовой стрелке
void GraphUtils::EnsureClockwise(Coordinates& polygon)
{
	if (!IsClockwise(polygon))
	{
		std::reverse(polygon.begin(), polygon.end());
	}
}

// Вспомогательная функция: получить индекс по координате
size_t GraphUtils::FindIndex(const Coordinates& C, const Point& p)
{
	// Предполагаем, что каждая точка уникальна
	auto it = std::find_if(C.begin(), C.end(), [&](auto& v) { return v.first == (int)p.x() && v.second == (int)p.y(); });
	return std::distance(C.begin(), it);
}