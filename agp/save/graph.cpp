#include "graph.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <queue>
#include <unordered_map>

Graph::Graph(const Matrix& matrix)
	: m_matrix(matrix)
{
}

Graph::Graph(int vetexCount, double value)
	: m_matrix(vetexCount, value)
{
}

std::vector<double>& Graph::operator[](int row)
{
	return m_matrix[row];
}

const std::vector<double>& Graph::operator[](int row) const
{
	return m_matrix[row];
}

int Graph::GetSize() const
{
	return m_matrix.GetSize();
}

Matrix Graph::GetMatrix() const
{
	return m_matrix;
}

void Graph::PrintGraph() const
{
	m_matrix.Print();
}

bool Graph::IsEmpty() const
{
	return m_matrix.IsEmpty();
}

void Graph::Draw(sf::RenderWindow& window)
{
	// window.draw(m_polygon);

	// Отрисовка рёбер
	sf::VertexArray edges(sf::Lines);
	for (size_t i = 0; i < m_matrix.GetSize(); ++i)
	{
		for (size_t j = i + 1; j < m_matrix.GetSize(); ++j)
		{
			if (m_matrix[i][j] == constants::EXIST)
			{
				// Добавляем вершины рёбер с учётом смещения
				edges.append(sf::Vertex(sf::Vector2f(static_cast<float>(m_coordinates[i].first) + m_offsetX,
											static_cast<float>(m_coordinates[i].second) + m_offsetY),
					color::WHITE));
				edges.append(sf::Vertex(sf::Vector2f(static_cast<float>(m_coordinates[j].first) + m_offsetX,
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
		m_text.setPosition(
			static_cast<float>(vertex.first) + m_offsetX - 4, static_cast<float>(vertex.second) + m_offsetY - 8);
		window.draw(m_text);
	}
}
