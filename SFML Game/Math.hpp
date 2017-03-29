#ifndef MATH_H
#define MATH_H

#include <SFML/System.hpp>
#include "Globals.hpp"

namespace Math {
	static const sf::Vector2f diff(const sf::Vector2f& i, const sf::Vector2f& j) {
		return i - j;
	};

	static const float dist(const sf::Vector2f& i, const sf::Vector2f& j) {
		sf::Vector2f d = i - j;
		return sqrtf(d.x * d.x + d.y * d.y);
	};

	static const float magnitude(const sf::Vector2f& v) {
		return sqrtf(v.x * v.x + v.y * v.y);
	}

	static const sf::Vector2f normalize(const sf::Vector2f& v) {
		return v / magnitude(v);
	}
	
	static const sf::Vector2f rotate(const sf::Vector2f& v, float rad) {
		float c = cosf(rad);
		float s = sinf(rad);
		return sf::Vector2f(v.x * c - v.y * s, v.x * s + v.y * c);
	}

	static const float angle(const sf::Vector2f& v) {
		return atan2f(v.y, v.x);
	};

	static const float angle(const sf::Vector2f& from, const sf::Vector2f& to) {
		return angle(diff(from, to));
	};

	static const sf::FloatRect boundingBox(const sf::FloatRect& a, const sf::FloatRect& b) {
		sf::FloatRect r;
		r.left = std::min(a.left, b.left);
		r.top = std::min(a.top, b.top);
		r.width = std::max(a.left + a.width, b.left + b.width) - r.left;
		r.height = std::max(a.top + a.height, b.top + b.height) - r.top;
		return r;
	};

	static const sf::Vector2f size(const sf::FloatRect& a) {
		return sf::Vector2f(a.width, a.height);
	};
	static const sf::Vector2f topleft(const sf::FloatRect& a) {
		return sf::Vector2f(a.left, a.top);
	};
	static const sf::Vector2f topright(const sf::FloatRect& a) {
		return sf::Vector2f(a.left + a.width, a.top);
	};
	static const sf::Vector2f bottomleft(const sf::FloatRect& a) {
		return sf::Vector2f(a.left, a.top + a.height);
	};
	static const sf::Vector2f bottomright(const sf::FloatRect& a) {
		return sf::Vector2f(a.left + a.width, a.top + a.height);
	};
	static const sf::Vector2f center(const sf::FloatRect& a) {
		return sf::Vector2f(a.left + a.width / 2.f, a.top + a.height / 2.f);
	};

	static const float toDeg(float rad) {
		const static float m = 180.f / PI_F;
		return rad * m;
	}
	static const float toRad(float deg) {
		const static float m = PI_F / 180.f;
		return deg * m;
	}
};

#endif