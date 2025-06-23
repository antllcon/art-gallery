#ifndef ADAPTER_H
#define ADAPTER_H

#include "graph.h"

/// @brief Класс адаптер
class FileToGraphAdapter
{
public:
	Graph ConvertEdgeListToMatrix(const std::string& fileName);

private:
	Matrix ReadMatrix(std::ifstream& file, size_t matrixSize) const;

	template <typename T>
	static T SafeRead(std::ifstream& file, const std::string& errorMessage);

	void AssertIsFileOpen(std::ifstream& file, const std::string& fileName) const;
	void AssertIsStreamCorrect(std::ifstream& file) const;
	void AssertIsValidSize(size_t size) const;
	void AssertIsValidNumbers(size_t from, size_t to, size_t matrixSize) const;
};

#endif // ADAPTER_H