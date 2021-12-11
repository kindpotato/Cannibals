#include "network.h"
#include <math.h>

float activation(float x) {
	return 1.f / (1.f + expf(-x));//sigmoid
}

void Network::init(const std::vector<unsigned int>& layerSizes, std::mt19937& generator) {
	const unsigned int depth = layerSizes.size() - 1;
	matricies.reserve(depth);

	for (int i = 0; i < depth; ++i) {
		Matrix mat(layerSizes[i + 1], layerSizes[i], generator);
		matricies.push_back(mat);
	}
	std::normal_distribution<float> normal{ 0, Matrix::STD_DEV };
	biases.resize(depth);
	for (int i = 0; i < biases.size(); ++i) {
		biases[i].reserve(layerSizes[i+1]);
		for (int j = 0; j < layerSizes[i+1]; ++j) {
			biases[i].push_back(normal(generator));
		}
	}
}

Network::Network(const std::vector<unsigned int>& layerSizes, std::mt19937& generator) {
	init(layerSizes, generator);
	
}
void Network::compute(const std::vector<float>& input, std::vector<float>& out) {
	if (input.size() != matricies[0].m) {
		std::cout << "Brain input wrong size" << std::endl;
		return;
	}
	std::vector<float> buffers[2];
	buffers[1] = input; //copies input into second buffer
	for (int i = 0; i < biases.size(); ++i) {
		matricies[i].times(buffers[1], buffers[0]);
		add(buffers[0], biases[i], buffers[1]);

		for (int j = 0; j < buffers[1].size();++j) {
			buffers[1][j] = activation(buffers[1][j]);
		}
	}
	//add(buffers[1], biases.back(), buffers[0]);//add final layer of biases
	out = buffers[1]; //copies final result into out.j
}

void Network::print() {
	for (int i = 0; i < biases.size(); ++i) {
		printVec(biases[i]);
		std::cout << std::endl;
		matricies[i].print();
		std::cout << std::endl;
	}
}

void Network::copy(const Network& other, std::mt19937 & generator) { //makes this brain the same as the other brain (plus random mutation)
	matricies = other.matricies;
	biases = other.biases;
	std::uniform_int_distribution<int> randMatrixIndex(0,matricies.size()-1);
	std::uniform_int_distribution<int> randBiasesIndex(0, biases.size()-1);
	int whichMatrix = randMatrixIndex(generator);
	matricies[whichMatrix].copy(other.matricies[whichMatrix],generator);
	int whichBiases = randBiasesIndex(generator);
	std::uniform_int_distribution<int> randBiasIndex(0, biases[whichBiases].size() - 1);
	std::normal_distribution<float> normal{ 0,Matrix::STD_DEV };
	int whichBias = randBiasIndex(generator);
	biases[whichBiases][whichBias] += normal(generator);

}
