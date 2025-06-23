#ifndef GRAPH_H
#define GRAPH_H

#include "../config.h"
#include "matrix.h"
#include <SFML/Graphics.hpp>
#include <fstream>
#include <stack>
#include <string>
#include <unordered_set>
#include <vector>

class Graph
{
public:
	Graph(const Matrix& matrix);
	Graph(int vetexCount, double value);
	~Graph() = default;

	std::vector<double>& operator[](int row);
	const std::vector<double>& operator[](int row) const;

	int GetSize() const;
	Matrix GetMatrix() const;
	void PrintGraph() const;
	bool IsEmpty() const;
	void Draw(sf::RenderWindow& window);

private:
	Matrix m_matrix;
	std::vector<int> m_holes;

	// SFML
	sf::ConvexShape m_polygon;
	sf::CircleShape m_vertex;
	sf::Font m_font;
	sf::Text m_text;
};

#endif // GRAPH_H