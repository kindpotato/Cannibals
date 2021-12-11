
#include <chrono>
#include <thread>
#include "dude.h"
#include "nearest.cpp"

//#define DUDE_COUNT 100




unsigned int DUDE_COUNT;

int main() {
	std::cout << "Enter movement cost: ";
	std::cin >> Dude::movementCost;
	std::cout << "Enter constant cost: ";
	std::cin >> Dude::metabolism;
	std::cout << "Enter standard deviation: ";
	std::cin >> Matrix::STD_DEV;
	std::cout << "Enter number of agents:";
	std::cin >> DUDE_COUNT;
	const sf::Vector2f windowDims(2000, 1500);
	sf::RenderWindow window(sf::VideoMode(windowDims.x/2.f,windowDims.y/2.f), "Cannibals");
	sf::View defaultView = window.getDefaultView();
	defaultView.setSize(windowDims);
	defaultView.setCenter(windowDims / 2.f);
	window.setView(defaultView);

	std::mt19937 generator(1);
	
	std::vector<Dude> dudes;
	dudes.reserve(DUDE_COUNT+1);
	std::uniform_real_distribution<float> xDistribution(0, windowDims.x);
	std::uniform_real_distribution<float> yDistribution(0, windowDims.y);
	for (int i = 0; i < DUDE_COUNT; ++i) {
		std::mt19937 gen(i + 1);
		dudes.push_back(Dude(gen));
		dudes[i].pos.x = xDistribution(generator);
		dudes[i].pos.y = yDistribution(generator);
	}
	Nearest nearest(DUDE_COUNT);
	
	sf::Texture ground;
	if (!ground.loadFromFile("ground.png")) {
		std::cout << "Could not load \"ground.png\"" << std::endl;
		return 1;
	}
	ground.setRepeated(true);
	
	int pauseFrame = 0;
	auto pauseTime = std::chrono::system_clock::now();
	int pauseDeaths = 0;

	unsigned long steps = 0;
	bool render = true;
	int champ = -1;
	int watching = -1;
	while (window.isOpen())
	{
		auto start = std::chrono::high_resolution_clock::now();
		nearest.treeDestroy();
		
		int highScore = 0;
		for (int i = 0; i < dudes.size(); ++i) {
			if (dudes[i].killCount > highScore) {
				highScore = dudes[i].killCount;
				champ = i;
			}
			nearest.addPoint(dudes[i].pos, i);
		}
		if (watching == -1) {
			watching = champ;
		}
		else {
			if (dudes[watching].dead()) {
				watching = champ;
			}
		}
		
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code) {
				case sf::Keyboard::P:
					render = render ^ 1;
					if (render == 0) {
						pauseFrame = steps;
						pauseTime = std::chrono::system_clock::now();
						pauseDeaths = Dude::deathToll;
					}
					else {
						std::chrono::duration<double> sincePause = std::chrono::system_clock::now() - pauseTime;
						std::cout << "Steps per second: " << (steps - pauseFrame) / sincePause.count() << '\n';
						std::cout << "Deaths per second: " << (Dude::deathToll - pauseDeaths) / sincePause.count() << std::endl;
					}
					
					std::cout << "Death Toll: " << Dude::deathToll << std::endl;

					break;
				case sf::Keyboard::Up:
					Matrix::STD_DEV += 0.05f;
					std::cout << Matrix::STD_DEV << std::endl;
					break;
				case sf::Keyboard::Down:
					Matrix::STD_DEV -= 0.05f;
					std::cout << Matrix::STD_DEV << std::endl;
					break;
				case sf::Keyboard::F:
					for (int i = 0; i < DUDE_COUNT; ++i) {
						dudes[i].print();
					}
					break;
				}
			}
		}
		for (int i = 0; i < dudes.size(); ++i) {//loops through all dudes
			if (dudes[i].dead()) {
				if (champ > -1) {
					dudes[i].copy(dudes[champ]);
					dudes[i].revive();
				}
				continue;
			}
		
			if (dudes[i].pos.x < 0.f) {
				dudes[i].pos.x = windowDims.x-0.1f;
			}
			else if (dudes[i].pos.x > windowDims.x) {
				dudes[i].pos.x = 0.1f;
			}
			if (dudes[i].pos.y < 0.f) {
				dudes[i].pos.y = windowDims.y-0.1f;
			}
			else if (dudes[i].pos.y > windowDims.y) {
				dudes[i].pos.y = 0.1f;
			}
			/*int a = -1, b = -1;
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
			if (a > -1 && b > -1) {
				dudes[i].update(dudes[a], dudes[b]);
			}
			*/
			
			nearest.findClosest(dudes[i].pos,windowDims);
			int ag = nearest.best;
			int bg = nearest.secondBest;

			if (dudes[i].killed(dudes[ag])) {
				dudes[ag].copy(dudes[i]);
				dudes[ag].revive();
			}
			if (dudes[i].killed(dudes[bg])) {
				dudes[bg].copy(dudes[i]);
				dudes[bg].revive();
			}
			if (ag > -1 && bg > -1 && ag < DUDE_COUNT && bg < DUDE_COUNT)
				dudes[i].update(dudes[ag], dudes[bg]);
		}
		for (int i = 0; i < DUDE_COUNT; ++i) {
			dudes[i].step();
		}

		
		if (render) {
			window.clear();
			for (int i = 0; i < dudes.size(); ++i) {
				dudes[i].draw(window);
			}
			if (watching > -1) {
				const float textureWidth = 100.f;
				float cells = 5;
				const sf::Vector2f blockSize(cells*textureWidth, cells * textureWidth);
				sf::Vector2f gridPos = dudes[watching].pos;
				gridPos /= textureWidth;
				gridPos = sf::Vector2f(floorf(gridPos.x), floorf(gridPos.y));
				gridPos *= textureWidth;
				gridPos.x -= 0.5f*cells*textureWidth;
				gridPos.y -= 0.5f *cells* textureWidth;
				sf::RectangleShape rect;
				rect.setSize(blockSize);
				rect.setPosition(gridPos);
				rect.setTexture(&ground);
				const float viewSize = 200.f;
				sf::View champView(dudes[watching].pos,sf::Vector2f(viewSize,viewSize));
				champView.setRotation(dudes[watching].angle*180.f/3.14159 + 90.f);
				sf::Vector2f pixel = sf::Vector2f(1.f/windowDims.x,1.f/windowDims.y);
				champView.setViewport(sf::FloatRect(0.f, 0.f, viewSize*pixel.x,viewSize*pixel.y));
				window.setView(champView);
				window.draw(rect);
				nearest.findClosest(dudes[watching].pos, windowDims);
				dudes[watching].draw(window);
				dudes[nearest.best].draw(window);
				dudes[nearest.secondBest].draw(window);
				window.setView(defaultView);
			}

			window.display();
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed = end - start;
			std::chrono::duration<double> period(std::chrono::microseconds(1000000/25));
			std::this_thread::sleep_for(period - elapsed);
		}
		
		++steps;
	}
	return 1;
}