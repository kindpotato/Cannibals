#include <SFML/Graphics.hpp>
#include <iostream>

template <class T>
class Space {

	unsigned int capacity;
	T* arr;
	unsigned int n;
public:

	Space(size_t cap) : n(0) {
		arr = new T[cap];
		capacity = cap;
	}
	~Space() {
		delete[] arr;
	}
	T* push(T element) {
		if (n == capacity) {
			std::cout << "Too many things" << std::endl;
			return nullptr;
		}
		//std::cout << "Add" << std::endl;
		arr[n] = element;
		return &arr[n++];
	}
	void clear() {
		n = 0;
	}
	unsigned int size() {
		return n;
	}
	T& operator [] (int a) {
		return arr[a];
	}

};

class Nearest {
	sf::Vector2f Q;
	struct BBox {
		float left, right, bottom, top;
		BBox trimLeft(bool h, sf::Vector2f pos) {
			BBox result = *this;
			if (h) right = pos.x;
			else top = pos.y;

			return result;
		}
		BBox trimRight(bool h, sf::Vector2f pos) {
			BBox result = *this;
			if (h) left = pos.x;
			else bottom = pos.y;

			return result;
		}
		float distance(sf::Vector2f point) {
			BBox temp = *this;
			temp.left -= point.x;
			temp.right -= point.x;
			temp.bottom -= point.y;
			temp.top -= point.y;
			sf::Vector2f to(0,0);
			if (temp.left * temp.right > 0)
				to.x = fmin(fabs(temp.left), fabs(temp.right));
			if (temp.bottom * temp.top > 0)
				to.y = fmin(fabs(temp.bottom), fabs(temp.top));
			return to.x * to.x + to.y * to.y;
		}
	};

	struct Node {
		sf::Vector2f pos;
		Node* left;
		Node* right;
		int id;
	};
	Space<Node> space;

public:
	Nearest(size_t cap):space(cap) {

	}
	void addPoint(sf::Vector2f point, int id) {
		Node* newNode = space.push(Node());
		newNode->pos = point;
		newNode->id = id;
		newNode->left = nullptr;
		newNode->right = nullptr;


		bool horizontal = true;
		Node** current = &root;
		while (*current) {
			if (horizontal) {
				if (newNode->pos.x < (*current)->pos.x) {
					current = &(*current)->left;
				}
				else {
					current = &(*current)->right;
				}
			}
			else {
				if (newNode->pos.y < (*current)->pos.y) {
					current = &(*current)->left;
				}
				else {
					current = &(*current)->right;
				}
			}
		}
		*current = newNode;
		
	}
private:

	// I mostly copied the function below from https://www.cs.cmu.edu/~ckingsf/bioinfo-lectures/kdtrees.pdf
	void NN(Node * T, bool h, BBox BB) {
		if (T == nullptr || BB.distance(Q) > secondDist) return;
		
		
		sf::Vector2f to = Q - T->pos;
		float dist = to.x * to.x + to.y * to.y;
		if (dist > 0.01) {
			if (dist < bestDist) {
				secondDist = bestDist;
				secondBest = best;
				bestDist = dist;
				best = T->id;
			}
			else if (dist < secondDist) {
				secondDist = dist;
				secondBest = T->id;
			}
		}
		bool comp = h ? Q.x < T->pos.x : Q.y < T->pos.y;
		if (comp){
			NN(T->left, !h, BB.trimLeft(h, T->pos));
			NN(T->right, !h, BB.trimRight(h, T->pos));
		}
		else {
			NN(T->right, !h, BB.trimRight(h, T->pos));
			NN(T->left, !h, BB.trimLeft(h, T->pos));
		}

	}
	
public:
	Node* root = nullptr;

	int best;
	int secondBest;
	float bestDist;
	float secondDist;


	void treeDestroy() {
		space.clear();
		root = nullptr;
	}


	void findClosest(sf::Vector2f point, sf::Vector2f roomDims) {
		best = -1;
		secondBest = -1;		
		bestDist = INFINITY;
		secondDist = INFINITY;
		BBox BB = { 0.f,roomDims.x,0.f,roomDims.y };
		Q = point;
		NN(root, true, BB);
	}
};