#include<iostream>
#include<vector>
#include<random>
#include<fstream>
#include<chrono>
#include<omp.h>
const int LOWER = -100; // Ќижн€€ граница диапазона с равномерным распределенем случайных чисел
const int UPPER = 100; // ¬ерхн€€ граница
std::vector<std::vector<int>> CreateMatrix(const int rows, const int cols) {
	std::vector<std::vector<int>> matrix(rows, std::vector<int>(cols));
	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_int_distribution<> distr(LOWER, UPPER);
#pragma omp parallel for
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			matrix[i][j] = distr(gen);
		}
	}
	return matrix;
}

std::vector<std::vector<int>> ReadMatrix(const std::string filename, int& rows, int& cols) {
	std::ifstream file(filename);
	if (!file.is_open()) throw std::runtime_error("File is not open!");
	file >> rows >> cols;
	std::vector<std::vector<int>> matrix(rows, std::vector<int>(cols));
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			file >> matrix[i][j];
		}
	}
	file.close();
	return matrix;
}

void WriteMatrix(const std::vector<std::vector<int>> matrix, const std::string filename) {
	std::ofstream file(filename);
	if (!file.is_open()) throw std::runtime_error("File is not open!");
	file << matrix.size() << " " << matrix[0].size() << std::endl;
	for (const auto& row : matrix) {
		for (const int& val : row) {
			file << val << " ";
		}
		file << std::endl;
	}
	file.close();
}

std::vector<std::vector<int>> MatrixMultiplication(const std::string& file1, const std::string& file2) {
	int rowsA, colsA, rowsB, colsB;
	std::vector<std::vector<int>> matrix1 = ReadMatrix(file1, rowsA, colsA);
	std::vector<std::vector<int>> matrix2 = ReadMatrix(file2, rowsB, colsB);
	if (colsA != rowsB)	throw std::runtime_error("");
	std::vector<std::vector<int>> result(rowsA, std::vector<int>(colsB, 0));
#pragma omp parallel for num_threads(16)
	for (int i = 0; i < rowsA; ++i) {
		for (int j = 0; j < colsB; ++j) {
			for (int k = 0; k < colsA; ++k) {
				result[i][j] += matrix1[i][k] * matrix2[k][j];
			}
		}
	}
	return result;
}

int main() {
	try {
		const int rows1 = 100;
		const int cols1 = 100;
		const int rows2 = 100;
		const int cols2 = 100;
		std::vector<std::vector<int>> matrix1 = CreateMatrix(rows1, cols1);
		std::vector<std::vector<int>> matrix2 = CreateMatrix(rows2, cols2);

		WriteMatrix(matrix1, "matrix1.txt");
		WriteMatrix(matrix2, "matrix2.txt");

		std::vector<std::vector<int>> result;
		auto start = std::chrono::high_resolution_clock::now();
		result = MatrixMultiplication("matrix1.txt", "matrix2.txt");
		auto end = std::chrono::high_resolution_clock::now();
		WriteMatrix(result, "result_matrix.txt");

		std::chrono::duration<double> duration = end - start;
		double meanTime = duration.count();
		std::cout << "The scope of the task: " << rows1 * cols1 + rows2 * cols2 << " elements." << std::endl;
		std::cout << "Execution time: " << meanTime << " seconds." << std::endl;
		return 0;
	}
	catch (const std::string err) {
		std::cout << err;
	}
}