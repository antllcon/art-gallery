#include "class-app/app.h"
#include "class-graph/graph.h"
#include "config.h"
#include <cstdlib>
#include <iostream>

// Алгоритм Гхоша для поиска охранников
Guards GetGuards(const Graph& graph)
{
	Guards guards;
	// Проведение линий через каждую пару вершин многоугольника
	// и вычисление всех компонент видимости
	auto viewComponents = graph.GetViewComponents();

	// TODO: Подумать как работать, стоит ли передавать сами объекты, а не их номера

	// Получение множества веерных многоугольников
	// добавляя те компоненты видимости, которые полностью видны из текущей вершины
	// auto funGrpahs = graph.GetFunGraphs(viewComponents);

	// Жадный алгоритм покрытия для поиска вершин
	// auto guards = graph.GetGuards(funGrpahs);

	// Получение координат вершин охранников
	// (возмжоно это не нужно, но на всякий случай)

	return guards;
}

std::string ParseArguments(int argc, char* argv[])
{
	if (argc != 2)
	{
		throw std::runtime_error("Формат ввода:\n[./museum-guards file_name]");
	}

	std::string fileName = argv[1];
	return fileName;
}

int main(int argc, char* argv[])
{
	try
	{
		// Создание графа из файла
		FileToGraphAdapter adapter;
		std::string fileName = ParseArguments(argc, argv);
		Graph artGallery = adapter.ConvertEdgeListToMatrix("../examples-polygon/" + fileName + ".txt");

		// Инициализация приложения
		sf::RenderWindow window(sf::VideoMode(screen::WIDTH, screen::HEIGHT), "Art Gallary Problem");
		App app;

		// Построим граф видимости
		artGallery.SetMatrix(artGallery.GetViewMatrix());

		// Вывод графа
		std::cout << artGallery << std::endl;

		// Поиск охранников
		// Guards guards = GetGuards(artGallery);

		// Цикл приложения
		while (window.isOpen())
		{
			app.ProcessEvents(window, artGallery);
			app.Render(window, artGallery);
		}
	}
	catch (const std::exception& errorMessage)
	{
		std::cerr << "Ошибка: " << errorMessage.what() << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "Программа завершена успешно" << std::endl;
	return EXIT_SUCCESS;
}