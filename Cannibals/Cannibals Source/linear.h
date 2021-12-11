#pragma once
#include <random>
#include <vector>
#include <iostream>
#define STD_DEV 0.2
void add(const std::vector<float> &, const std::vector<float> &, std::vector<float>&);
void printVec(std::vector<float> vec);

class Matrix {
	std::vector<float> entries;

public:
	unsigned int n, m;
	Matrix(unsigned int, unsigned int, std::mt19937 &);
	Matrix(Matrix &,std::mt19937 &);
	void times(std::vector<float> const &, std::vector<float> &);
	void print();
	void copy(const Matrix&, std::mt19937&);
};