#include "dude.h"
#include "network.h"
#define pi 3.1415926538f

float mag(sf::Vector2f v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

void Dude::update(Dude& a, Dude& b) {
	std::vector<float> senses{vel.x,vel.y,angle,pos.x,pos.y,a.angle,a.pos.x - pos.x,a.pos.y - pos.y,b.angle,b.pos.x - pos.x,b.pos.y - pos.y };
	std::vector<float> out;

	brain.compute(senses, out);

	if (out.size() != 3) {
		std::cout << out.size() << std::endl;
	}
	const float movingAngle = pi*2.3f*(out[0] - 0.5f);
	vel += 0.1f*out[1]*sf::Vector2f(cos(movingAngle),sin(movingAngle));
	energy -= out[1]*mag(vel);
	//angularVel += 0.05f * out[2];
	angle = pi*2.3f*(out[2]-0.5f);
	vel *= 0.8f;
	//angularVel *= 0.8f;

	pos += vel;
	//angle += angularVel;

	

	angle = fmod(angle, 2.f * pi);
	//angularVel = fmod(angularVel+pi, 2.f * pi)-pi;
}

void Dude::draw(sf::RenderWindow& window) {
	const float rad = RADIUS;
	shape[0].position = pos + (2.f*rad*sf::Vector2f(cos(angle), sin(angle)));
	const float space = 2.f * pi / (float)VERT_COUNT;
	float around = space;
	for (int i = 1; i < VERT_COUNT; ++i) {
		shape[i].position = pos + (rad* sf::Vector2f(cos(angle+around), sin(angle+around)));
		around += space;
	}
	window.draw(shape);
}

void Dude::copy(const Dude& other) {
	alive = true;
	brain.copy(other.brain,generator);
	shape[0].color = sf::Color::Green;
	for (int i = 1; i < shape.getVertexCount(); ++i) {
		shape[i].color = sf::Color::White;
	}
	std::uniform_real_distribution<float> place(-100, 100);
	pos = sf::Vector2f(300,200)+sf::Vector2f(place(generator),place(generator));
	energy += 1000.f;
}
bool Dude::killed(const Dude& other) {
	const float rad = RADIUS;
	sf::Vector2f swordTip((2.f*rad*sf::Vector2f(cos(angle),sin(angle)))+pos);
	sf::Vector2f to(other.pos - swordTip);

	bool murdered = (to.x * to.x + to.y * to.y) < (rad*rad);
	if (murdered)
		energy += 1000.f;
	return murdered;
}

bool Dude::dead() {
	if (energy < 0 && alive) {
		for (int i = 0; i < shape.getVertexCount(); ++i) {
			shape[i].color = sf::Color::Red;
		}
		alive = false;
	}
	return !alive;
}