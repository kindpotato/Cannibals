#include "linear.h"

void printVec(std::vector<float> vec) {
	for (float f : vec) {
		std::cout << f << "  ";
	}
	std::cout << std::endl;
}



void add(const std::vector<float>& a, const std::vector<float>& b, std::vector<float>&out) {
	if (a.size() != b.size()) {
		std::cout << "addition failed" << std::endl;
		return;
	}
	out.clear();
	out.reserve(a.size());
	for (int i = 0; i < a.size(); ++i)
		out.push_back(a[i] + b[i]);
}


Matrix::Matrix(unsigned int n, unsigned int m, std::mt19937 & generator){
	entries.reserve(n * m);
	this->n = n;
	this->m =  m;
	std::normal_distribution<float> dist{ 0,STD_DEV };
	for (int i = 0; i < n * m; ++i) {
		entries.push_back(dist(generator));
	}
}
Matrix::Matrix(Matrix & mat, std::mt19937 & generator) {
	entries.reserve(mat.n*mat.m);
	n = mat.n;
	m = mat.m;
	/*for (int i = 0; i < m * n; ++i) {
		entries[i] = mat.entries[i];
	}*/
	entries = mat.entries;
	std::uniform_int_distribution<int> uni(0, mat.n * mat.m-1);
	std::normal_distribution<float> normal{ 0,STD_DEV };
	entries[uni(generator)] += normal(generator);
}

void Matrix::times(std::vector<float> const& vec, std::vector<float> & out) {
	if (vec.size() != m) {
		std::cout << "multiplication failed" << std::endl;
		return;
	}
	out.clear();
	out.resize(n);
	for (int i = 0; i < n; ++i) {
		float sum = 0;
		for (int j = 0; j < m; ++j) {
			sum += entries[i * m + j]*vec[j];
		}
		out[i] = sum;
	}
}

void Matrix::copy(const Matrix& other, std::mt19937& generator) {
	entries = other.entries;
	std::uniform_int_distribution<int> pos(0, entries.size()-1);
	std::normal_distribution<float> diff(0, 1);
	entries[pos(generator)] += diff(generator);
}

void Matrix::print() {
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			std::cout << entries[i * m + j] << "  ";
		}
		std::cout << std::endl;
	}
}
