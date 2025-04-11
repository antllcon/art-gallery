#include <iostream>
#include <memory>
#include <vector>

struct Edge;
struct Vertex;

using EdgePtr = std::shared_ptr<Edge>;
using VertexPtr = std::shared_ptr<Vertex>;

struct Vertex
{
	unsigned int x, y;

	Vertex(unsigned int x = 0, unsigned int y = 0)
		: x(x)
		, y(y)
	{
	}

	friend std::ostream& operator<<(std::ostream& os, const Vertex& v)
	{
		return os << "(" << v.x << ", " << v.y << ")";
	}
};

struct Edge
{
	std::vector<VertexPtr> nodes;

	Edge(VertexPtr v1, VertexPtr v2)
		: nodes{v1, v2}
	{
	}

	bool IsСontains(VertexPtr v) const
	{
		return nodes[0] == v || nodes[1] == v;
	}

	VertexPtr GetOpposite(VertexPtr v) const
	{
		if (nodes[0] == v)
			return nodes[1];
		if (nodes[1] == v)
			return nodes[0];
		return nullptr;
	}
};

class Graph
{
	std::vector<VertexPtr> vertices;
	std::vector<EdgePtr> edges;

public:
	// Добавление вершины
	VertexPtr AddVertex(unsigned int x, unsigned int y)
	{
		auto v = std::make_shared<Vertex>(x, y);
		vertices.push_back(v);
		return v;
	}

	// Добавление ребра
	EdgePtr AddEdge(VertexPtr v1, VertexPtr v2)
	{
		for (const auto& edge : edges)
		{
			if ((edge->nodes[0] == v1 && edge->nodes[1] == v2) || (edge->nodes[0] == v2 && edge->nodes[1] == v1))
			{
				return edge;
			}
		}

		auto e = std::make_shared<Edge>(v1, v2);
		edges.push_back(e);
		return e;
	}

	// Поиск всех вершин
	const std::vector<VertexPtr>& getVertices() const { return vertices; }

	// Поиск всех рёбер
	const std::vector<EdgePtr>& getEdges() const { return edges; }
};