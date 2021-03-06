
#include <math.h>

#include "util/Math.hpp"
#include "game/lvl/Tile.hpp"

const float PI_F = 3.14159265358979323846f;

const sf::Vector2f Math::diff(const sf::Vector2f& i, const sf::Vector2f& j) {
	return i - j;
};

float Math::dist(const sf::Vector2f& i, const sf::Vector2f& j) {
	sf::Vector2f d = i - j;
	return sqrtf(d.x * d.x + d.y * d.y);
};

float Math::magnitude(const sf::Vector2f& v) {
	return sqrtf(v.x * v.x + v.y * v.y);
}

float Math::lerp(float a, float b, float perc) {
	return ((1.f - perc) * a) + (perc * b);
};

const sf::Vector2f Math::lerp(sf::Vector2f a, sf::Vector2f b, float perc) {
	return sf::Vector2f(lerp(a.x, b.x, perc), lerp(a.y, b.y, perc));
};

const sf::Vector2f Math::normalize(const sf::Vector2f& v) {
	return v / magnitude(v);
}

const sf::Vector2f Math::rotate(const sf::Vector2f& v, float rad) {
	float c = cosf(rad);
	float s = sinf(rad);
	return sf::Vector2f(v.x * c - v.y * s, v.x * s + v.y * c);
}

float Math::angle(const sf::Vector2f& v) {
	return atan2f(v.y, v.x);
};

float Math::angle(const sf::Vector2f& from, const sf::Vector2f& to) {
	return angle(diff(from, to));
};

const sf::FloatRect Math::boundingBox(const sf::FloatRect& a, const sf::FloatRect& b) {
	sf::FloatRect r;
	r.left = std::min(a.left, b.left);
	r.top = std::min(a.top, b.top);
	r.width = std::max(a.left + a.width, b.left + b.width) - r.left;
	r.height = std::max(a.top + a.height, b.top + b.height) - r.top;
	return r;
};

const sf::FloatRect Math::lerpBox(const sf::FloatRect& a, const sf::FloatRect& b, float perc) {
	sf::Vector2f lerpSize = lerp(size(a), size(b), perc);
	sf::Vector2f lerpCenter = lerp(center(a), center(b), perc);

	return sf::FloatRect(lerpCenter - lerpSize * 0.5f, lerpSize);

};

const sf::IntRect Math::gridBounds(const sf::FloatRect& a) {
	sf::IntRect r;
	r.left = (int)(a.left / tileSpacing);
	r.width = (int)ceilf((a.left + a.width) / tileSpacing) - r.left;
	r.top = (int)(a.top / tileSpacing);
	r.height = (int)ceilf((a.top + a.height) / tileSpacing) - r.top;
	return r;
};

const sf::Vector2f Math::size(const sf::FloatRect& a) {
	return sf::Vector2f(a.width, a.height);
};
const sf::Vector2f Math::topleft(const sf::FloatRect& a) {
	return sf::Vector2f(a.left, a.top);
};
const sf::Vector2f Math::topright(const sf::FloatRect& a) {
	return sf::Vector2f(a.left + a.width, a.top);
};
const sf::Vector2f Math::bottomleft(const sf::FloatRect& a) {
	return sf::Vector2f(a.left, a.top + a.height);
};
const sf::Vector2f Math::bottomright(const sf::FloatRect& a) {
	return sf::Vector2f(a.left + a.width, a.top + a.height);
};
const sf::Vector2f Math::center(const sf::FloatRect& a) {
	return sf::Vector2f(a.left + a.width / 2.f, a.top + a.height / 2.f);
};

float Math::toDeg(float rad) {
	const  float m = 180.f / PI_F;
	return rad * m;
}
float Math::toRad(float deg) {
	const  float m = PI_F / 180.f;
	return deg * m;
}
