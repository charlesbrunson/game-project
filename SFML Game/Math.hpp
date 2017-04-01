#ifndef MATH_H
#define MATH_H

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

extern const float PI_F;

const enum Cardinal {
	NO_DIR = -1,
	NORTH = 0,
	EAST,
	SOUTH,
	WEST,
	DIR_COUNT
};

namespace Math {
	const sf::Vector2f diff(const sf::Vector2f& i, const sf::Vector2f& j);

	const float dist(const sf::Vector2f& i, const sf::Vector2f& j);

	const float magnitude(const sf::Vector2f& v);

	const sf::Vector2f normalize(const sf::Vector2f& v);
	
	const sf::Vector2f rotate(const sf::Vector2f& v, float rad);

	const float angle(const sf::Vector2f& v);
	const float angle(const sf::Vector2f& from, const sf::Vector2f& to);

	const sf::FloatRect boundingBox(const sf::FloatRect& a, const sf::FloatRect& b);

	const sf::Vector2f size(const sf::FloatRect& a);
	const sf::Vector2f topleft(const sf::FloatRect& a);
	const sf::Vector2f topright(const sf::FloatRect& a);
	const sf::Vector2f bottomleft(const sf::FloatRect& a);
	const sf::Vector2f bottomright(const sf::FloatRect& a);
	const sf::Vector2f center(const sf::FloatRect& a);

	const float toDeg(float rad);
	const float toRad(float deg);
};

#endif