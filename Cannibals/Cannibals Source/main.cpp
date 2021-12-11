
#include <chrono>
#include "dude.h"

#define DUDE_COUNT 20

int main() {
	/*auto start = std::chrono::high_resolution_clock::now();
	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "Microseconds: " << elapsed.count()*2000000 << std::endl;*/
	const sf::Vector2f windowDims(800, 600);
	sf::RenderWindow window(sf::VideoMode(windowDims.x,windowDims.y), "SFML works!");
	std::mt19937 generator(1);
	std::vector<Dude> dudes;
	dudes.reserve(DUDE_COUNT);
	std::uniform_real_distribution<float> xDistribution(0, windowDims.x);
	std::uniform_real_distribution<float> yDistribution(0, windowDims.y);
	for (int i = 0; i < DUDE_COUNT; ++i) {
		std::mt19937 gen(i + 1);
		dudes.push_back(Dude(gen));
		dudes[i].pos.x = xDistribution(generator);
		dudes[i].pos.y = yDistribution(generator);
	}

	
	
	unsigned long steps = 0;
	bool render = true;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
				case sf::Keyboard::P:
					render = render ^ 1;
					std::cout << steps << std::endl;
					break;
				}
			}
		}
		
		const float bounce = 0.f;
		for (int i = 0; i < dudes.size(); ++i) {//loops through all dudes
			if (dudes[i].dead())
				continue;
			if (dudes[i].pos.x < 0.f) {
				dudes[i].pos.x = windowDims.x-0.1f;
				//dudes[i].pos.x = 0.1f;
				//dudes[i].vel.x = bounce+abs(dudes[i].vel.x);
			}
			else if (dudes[i].pos.x > windowDims.x) {
				dudes[i].pos.x = 0.1f;
				//dudes[i].pos.x = windowDims.x-0.1f;
				//dudes[i].vel.x = -bounce-abs(dudes[i].vel.x);
			}
			if (dudes[i].pos.y < 0.f) {
				dudes[i].pos.y = windowDims.y-0.1f;
				//dudes[i].pos.y = 0.1f;
				//dudes[i].vel.y = bounce+abs(dudes[i].vel.y);
			}
			else if (dudes[i].pos.y > windowDims.y) {
				dudes[i].pos.y = 0.1f;
				//dudes[i].pos.y = windowDims.y-0.1f;
				//dudes[i].vel.y = -bounce-abs(dudes[i].vel.y);
			}
			
			int a = -1, b = -1;
			float aDist = 1000000.f, bDist = 1000000.f;

			for (int j = 0; j < dudes.size(); ++j) {
				if (j == i)
					continue;
				if (dudes[i].killed(dudes[j])) {
					dudes[j].copy(dudes[i]);
				}

				sf::Vector2f to = dudes[i].pos - dudes[j].pos;
				float len = to.x * to.x + to.y * to.y;
				if (len < aDist) {
					b = a;
					bDist = aDist;
					a = j;
					aDist = len;
				}
				else if (len < bDist) {
					b = j;
					bDist = len;
				}
			}
			if (a > -1 && b > -1)
				dudes[i].update(dudes[a], dudes[b]);
		}
		if (render) {
			window.clear();
			for (int i = 0; i < dudes.size(); ++i) {
				dudes[i].draw(window);
			}
			window.display();
		}
		
		++steps;
	}
	return 1;
}