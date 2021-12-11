#pragma once
#ifndef GRAPHICS
#define GRAPHICS
#include <SFML/Graphics.hpp>
#endif
#include "network.h"
#define VERT_COUNT 6
#define RADIUS 10.f;


class Dude {
	sf::VertexArray shape;
	Network brain;
	std::mt19937 generator;
	
	float angularVel = 0.f;
	float energy = 1000.f;
	bool alive = true;
	void collide(const Dude&);
	sf::Color color;
public:
	float angle = 0.f;
	int killCount = 0;
	sf::Vector2f pos;
	sf::Vector2f vel;
	static float movementCost;
	static float metabolism;
	static int deathToll;
	sf::Vector2f move;
	float twist;
	sf::Vector2f force;
	float angleForce;
	Dude(std::mt19937& generator);
	void print();
	
	void copy(const Dude&);
	void revive();
	void update(Dude&, Dude&);
	void draw(sf::RenderWindow& window);
	bool killed(Dude&);
	bool dead();
	void step();
};
