#ifndef ADJACENCY_MATRIX_H
#define ADJACENCY_MATRIX_H

#include <iostream>
#include <stdexcept>
#include <vector>

using Data = std::vector<std::vector<double>>;

class Matrix
{
public:
	Matrix() = default;
	Matrix(Data data);
	Matrix(int size, double value = 0.0);

	std::vector<double>& operator[](int row);
	const std::vector<double>& operator[](int row) const;

	int GetSize() const;
	bool IsEmpty() const;

	double Get(int row, int col) const;
	void Set(int row, int col, double value);

	void Print() const;
	const Data& GetData() const;

protected:
	void AssertIsMatrixSize(int vertexCount) const;
	void AssertIsMatrixSquare() const;

private:
	Data m_data;
};

#endif // ADJACENCY_MATRIX_H