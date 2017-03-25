#ifndef MATH_H
#define MATH_H

#include <SFML/System.hpp>

namespace Math {
	static sf::Vector2f diff(sf::Vector2f i, sf::Vector2f j) {
		return i - j;
	};
	static float dist(sf::Vector2f i, sf::Vector2f j) {
		sf::Vector2f d = diff(i, j);
		return d.x * d.x + d.y * d.y;
	};
	static float angle(sf::Vector2f from, sf::Vector2f to) {
		sf::Vector2f d = diff(from, to);
		return atanf(d.x / d.y);
	};

	static sf::FloatRect boundingBox(sf::FloatRect a, sf::FloatRect b) {
		return sf::FloatRect(
			std::min(a.left, b.left),
			std::min(a.top, b.top),
			std::max((a.left + a.width) - b.left, (b.left + b.width) - a.left),
			std::max((a.top + a.height) - b.top, (b.top + b.height) - a.top)
			);
	};
};

#endif