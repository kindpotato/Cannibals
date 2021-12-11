#pragma once
#include <SFML/Graphics.hpp>
#include "network.h"
#define VERT_COUNT 10
#define RADIUS 10.f;
class Dude {
	sf::VertexArray shape;
	Network brain;
	std::mt19937 generator;
	float angle = 0.f;
	float angularVel = 0.f;
	float energy = 1000.f;
	bool alive = true;
public:
	sf::Vector2f pos;
	sf::Vector2f vel;
	//sf::Vector2f force;
	Dude(std::mt19937 & generator) {
		this->generator = generator;
		std::vector<unsigned int> layerSizes{ 11,8,5,3 };
		brain.init(layerSizes, generator);
		shape.setPrimitiveType(sf::TrianglesFan);
		shape.resize(VERT_COUNT);
		
		pos = sf::Vector2f(0.f, 0.f);
		vel = sf::Vector2f(0.f, 0.f);
		//force = sf::Vector2f(0.f, 0.f);
		shape[0].color = sf::Color::Green;
		for (int i = 1; i < VERT_COUNT; ++i) {
			shape[i].color = sf::Color::White;
		}
	}
	
	void copy(const Dude&);
	void update(Dude&, Dude&);
	void draw(sf::RenderWindow& window);
	bool killed(const Dude&);
	bool dead();
};