#pragma once
#ifndef GRAPHICS
#define GRAPHICS
#include <SFML/Graphics.hpp>
#endif
#include "linear.h"

class Network {
	std::vector <Matrix> matricies;
	std::vector<std::vector<float>> biases;
public:
	static std::vector<float> layerSizes;
	Network(const std::vector<unsigned int>&, std::mt19937&);
	Network() {} //does nothing
	void compute(const std::vector<float>&, std::vector<float>&);
	void print();
	void init(const std::vector<unsigned int>&, std::mt19937&);
	void copy(const Network&, std::mt19937 &);

};