#include "dude.h"
#include "network.h"
#define pi 3.1415926538f

float mag(sf::Vector2f v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

float dot(sf::Vector2f a, sf::Vector2f b) {
	return a.x * b.x + a.y * b.y;
}

float angleDiff(float a, float b) {
	float result = b - a;
	if (result > pi) result -= 2.f * pi;
	else if (result < pi) result += 2.f * pi;
	return result;
}

float colorDiff(sf::Color a, sf::Color b) {
	return abs(a.r - b.r) + abs(a.g - b.g) + abs(a.b - b.b);
}

sf::Vector2f relativize(sf::Vector2f a, sf::Vector2f b, sf::Vector2f X, sf::Vector2f Y){
	sf::Vector2f result = b - a;
	result = sf::Vector2f(dot(result, X), dot(result, Y));
	return result;
}

void printColor(sf::Color col) {

	std::cout << (sf::Uint32)col.r << ' ' << (sf::Uint32)col.g << ' ' << (sf::Uint32)col.b << '\n';
}

float Dude::movementCost = 0.3f;
float Dude::metabolism = 0.5;
int Dude::deathToll = 0;
Dude::Dude(std::mt19937& generator) {
	this->generator = generator;
	std::vector<unsigned int> layerSizes{ 13,10,6,3 };
	brain.init(layerSizes, generator);
	shape.setPrimitiveType(sf::TrianglesFan);
	shape.resize(VERT_COUNT);
	color = sf::Color(128, 128, 128);
	pos = sf::Vector2f(0.f, 0.f);
	vel = sf::Vector2f(0.f, 0.f);
	force = sf::Vector2f(0.f, 0.f);
	angleForce = 0.f;
	shape[0].color = sf::Color::Green;
	for (int i = 1; i < VERT_COUNT; ++i) {
		shape[i].color = sf::Color::White;
	}
}
void Dude::update(Dude& a, Dude& b) {
	sf::Vector2f X(cos(angle), sin(angle));
	sf::Vector2f Y(-X.y, X.x);
	sf::Vector2f myVel(dot(vel, X), dot(vel, Y));

	sf::Vector2f aPos = relativize(pos,a.pos,X,Y);
	sf::Vector2f bPos = relativize(pos, b.pos, X, Y);
	sf::Vector2f aVel = relativize(vel, a.vel, X, Y);
	sf::Vector2f bVel = relativize(vel, b.vel, X, Y);

	//float adiff = colorDiff(color,a.color);
	//float bdiff = colorDiff(color, b.color);
	std::vector<float> senses{myVel.x,myVel.y,angleDiff(angle,a.angle),aPos.x,aPos.y,angleDiff(angle,b.angle),bPos.x, bPos.y, aVel.x,aVel.y, bVel.x, bVel.y, angularVel};
	std::vector<float> out;

	brain.compute(senses, out);

	if (out.size() != 3) {
		std::cout << out.size() << std::endl;
	}
	move = 2.f*sf::Vector2f(out[0]-0.5f, out[1]-0.5f);
	sf::Vector2f push = 1.2f*((move.x*X) + (move.y*Y));
	force += push;
	energy -= movementCost*(fabs(dot(push,vel)))+metabolism;
	
	

	twist = out[2] - 0.5f;
	angleForce += 0.3f * twist;
	if (angularVel*twist > 0)
		energy -= 120.f*movementCost*twist*angularVel;
	
	collide(a);
	collide(b);
}

void Dude::step() {
	//Friction
	vel *= 0.8f;
	angularVel *= 0.7f;
	angularVel += angleForce;
	vel += force;
	//Change position and angle
	angle += angularVel;
	angle = fmod(angle, 2.f * pi);
	pos += vel;
	//set forces to 0
	force = sf::Vector2f(0.f, 0.f);
	angleForce = 0.f;
}

void Dude::draw(sf::RenderWindow& window) {
	if (shape.getVertexCount() == 0) return;
	const float rad = RADIUS;
	shape[0].position = pos + (2.f*rad*sf::Vector2f(cos(angle), sin(angle)));
	const float SPACE = 2.f * pi / (float)VERT_COUNT;
	float around = SPACE;
	if (dead()) {
		shape[0].color = sf::Color::Red;
		shape[VERT_COUNT / 2].color = sf::Color::Red;
	}
	else {
		shape[0].color = color;
		shape[VERT_COUNT / 2].color = sf::Color::White;
	}
	for (int i = 1; i < VERT_COUNT; ++i) {
		shape[i].position = pos + (rad* sf::Vector2f(cos(angle+around), sin(angle+around)));
		shape[i].color = color;
		around += SPACE;
	}
	window.draw(shape);
}

void Dude::copy(const Dude& other) {
	brain.copy(other.brain,generator);
	std::uniform_int_distribution<int> dist(0, 2);
	std::uniform_int_distribution<int> dist1(-20,20);
	signed char amount = dist1(generator);
	color = other.color;
	sf::Uint8* channel = nullptr;
	switch (dist(generator)) {
	case 0:
		channel = &color.r;
		break;
	case 1:
		channel = &color.g;
		break;
	case 2:
		channel = &color.b;
		break;
	}
	int sum = (int)(*channel) + (int)amount;
	if (sum > 0xFF) {
		(*channel) = 0xFF;
	}
	else if (sum < 0) {
		(*channel) = 0;
	}
	else
		(*channel) += amount;

}

void Dude::revive() {
	killCount = 0;
	alive = true;
	std::uniform_real_distribution<float> place(-100, 100);
	pos += sf::Vector2f(place(generator), place(generator));
	energy += 1000.f;
}

bool Dude::killed(Dude& other) {
	const float rad = RADIUS;
	sf::Vector2f swordTip((2.f*rad*sf::Vector2f(cos(angle),sin(angle)))+pos);
	sf::Vector2f to(other.pos - swordTip);

	bool stabbed = (to.x * to.x + to.y * to.y) < (rad*rad);
	bool murdered = false;
	if (stabbed) {
		other.energy -= 200.f;
		
		if (other.dead()) {
			energy += 1000.f;
			++killCount;
			++deathToll;
			murdered = true;
		}
	}
	return murdered;
}

bool Dude::dead() {
	if (energy < 0 && alive) {
		killCount = 0;
		++deathToll;
		alive = false;
	}
	return !alive;
}
void Dude::collide(const Dude& other) {
	const float rad = RADIUS;
	sf::Vector2f to = other.pos - pos;
	float distSqrd = dot(to, to);
	if (distSqrd < 4.f * rad*rad && distSqrd != 0.f) {
		float dist = sqrtf(distSqrd);
		to /= dist;
		float spring = 0.2*(2.f * rad - dist);
		sf::Vector2f();
		float dampen = 0.6*dot(vel - other.vel, to);
		force -= (spring+dampen) * to;
	}
}

void Dude::print() {
	std::cout << vel.x << ' ' << vel.y << "\nEnergy: " << energy << std::endl;
}