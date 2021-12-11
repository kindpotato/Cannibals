#pragma once
#include "linear.h"

class Network {
	std::vector <Matrix> matricies;
	std::vector<std::vector<float> > biases;
public:
	Network(const std::vector<unsigned int>&, std::mt19937&);
	Network() {} //does nothing
	void compute(const std::vector<float>&, std::vector<float>&);
	void print();
	void init(const std::vector<unsigned int>&, std::mt19937&);
	void copy(const Network&, std::mt19937 &);

};