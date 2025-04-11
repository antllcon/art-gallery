#include "class-graph/graph.h"
#include "config.h"
#include <cstdlib>
#include <iostream>
#include <set>

Graph GetCompleteGraph(const Graph& graph)
{
	const auto graphSize = graph.GetVertexCount();
	Graph completeGraph(graphSize, false);

	for (size_t i = 0; i < graphSize; ++i)
	{
		for (size_t j = 0; j < graphSize; ++j)
		{
			completeGraph.AddEdge(i, j);
		}
	}

	return completeGraph;
}

Components FindComponents(const Graph& completeGraph)
{
}

Component GetGuards(const Graph& graph)
{
	Component guards;
	Graph completeGraph = GetCompleteGraph(graph);

	// TODO: сделать класс графа с координатами вершин!
	Components visibileComponents = FindComponents(completeGraph);

	// логика определния охранников

	return guards;
}

int main()
{
	try
	{
		AdjacencyMatrix matrix = {
			{0, 1, 0, 0, 1},
			{1, 0, 1, 0, 0},
			{0, 1, 0, 1, 0},
			{0, 0, 1, 0, 1},
			{1, 0, 0, 1, 0}};
		Graph graph(matrix);
		Component guards = GetGuards(graph);
	}
	catch (const std::exception& errorMessage)
	{
		std::cerr << errorMessage.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}