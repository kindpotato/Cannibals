#pragma once

#ifndef GRAPHICS
#define GRAPHICS
#include <SFML/Graphics.hpp>
#endif

class Bullet
{
	sf::Vector2f pos;
	sf::Vector2f velocity;
	void draw(sf::RenderWindow& window);
};

