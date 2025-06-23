#include "matrix.h"

Matrix::Matrix(Data data)
	: m_data(data)
{
	AssertIsMatrixSize(GetSize());
	AssertIsMatrixSquare();
}

Matrix::Matrix(int size, double value)
{
	AssertIsMatrixSize(size);
	m_data.resize(size, std::vector<double>(size, 0.0));

	if (value == 0.0)
	{
		return;
	}

	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			if (i != j)
				m_data[i][j] = value;
		}
	}

	AssertIsMatrixSquare();
}

int Matrix::GetSize() const
{
	return static_cast<int>(m_data.size());
}

bool Matrix::IsEmpty() const
{
	return m_data.empty();
}

std::vector<double>& Matrix::operator[](int row)
{
	if (row < 0 || row >= GetSize())
		throw std::out_of_range("Row index out of range");
	return m_data[row];
}

const std::vector<double>& Matrix::operator[](int row) const
{
	if (row < 0 || row >= GetSize())
		throw std::out_of_range("Row index out of range");
	return m_data[row];
}

double Matrix::Get(int row, int col) const
{
	return m_data[row][col];
}

void Matrix::Set(int row, int col, double value)
{
	m_data[row][col] = value;
}

void Matrix::Print() const
{
	for (const auto& row : m_data)
	{
		for (double val : row)
		{
			std::cout << val << " ";
		}
		std::cout << std::endl;
	}
}

const Data& Matrix::GetData() const
{
	return m_data;
}

void Matrix::AssertIsMatrixSize(int vertexCount) const
{
	if (vertexCount <= 0)
	{
		throw std::invalid_argument("Матрица смежности не может быть пустой!");
	}
}

void Matrix::AssertIsMatrixSquare() const
{
	for (const auto& row : m_data)
	{
		if (row.size() != GetSize())
		{
			throw std::invalid_argument("Матрица смежности должна быть квадратной!");
		}
	}
}