
#ifndef GRIDVECTOR_H
#define GRIDVECTOR_H

#include <SFML/System.hpp>

class GridVector : public sf::Vector2i {
public:
	GridVector() : sf::Vector2i() {}
	GridVector(sf::Vector2i v) : sf::Vector2i(v) {}
	GridVector(int a, int b) : sf::Vector2i(a, b) {}
	GridVector(GridVector *a) {
		x = a->x;
		y = a->y;
	}

	friend inline bool operator< (const GridVector& l, const GridVector& r);
};

inline bool operator< (const GridVector& l, const GridVector& r) {
	if (l.x > r.x) {
		return false;
	}
	else if (l.x < r.x) {
		return true;
	}
	else {
		if (l.y > r.y) {
			return false;
		}
		else if (l.y < r.y) {
			return true;
		}
	}
	return false;
}
#endif