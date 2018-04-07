
#include <math.h>
#include <assert.h>

#include "util/Math.hpp"
#include "game/lvl/Tile.hpp"

const float PI_F = 3.14159265358979323846f;

const sf::Vector2f Math::diff(const sf::Vector2f& i, const sf::Vector2f& j) {
	return i - j;
};

float Math::pointOnLine(const Line& line, const Point& p) {
	return (line.end.y - line.start.y) * p.x +
		   (line.start.x - line.end.x) * p.y +
		   (line.end.x * line.start.y - line.start.x * line.end.y);
};

bool Math::intersects(const Line& line, const sf::FloatRect& rect, bool lineIsSegment) {
	if (line.isHorizontal()) {
		return line.start.y > rect.top && line.start.y < rect.top + rect.height &&
			(!lineIsSegment || (rect.left + rect.width >= std::min(line.start.x, line.end.x) &&
			rect.left <= std::max(line.start.x, line.end.x)));

	}
	else if (line.isVertical()) {
		
		return line.start.x > rect.left && line.start.x < rect.left + rect.width &&
			(!lineIsSegment || (rect.top + rect.height > std::min(line.start.y, line.end.y) &&
			rect.top <= std::max(line.start.y, line.end.y)));

	}
	else if (Math::boundingBox(line.start, line.end).intersects(rect)) {
		auto isAboveLine =
		[](const Line& line, const Point& p) -> bool {
			float r = Math::pointOnLine(line, p);
			return r < 0.f;
		};

		bool t = isAboveLine(line, Math::topleft(rect));

		return t == isAboveLine(line, Math::topright(rect)) &&
		       t == isAboveLine(line, Math::bottomleft(rect)) &&
		       t == isAboveLine(line, Math::bottomright(rect));

	}
	else {
		return false;
	}
};

Vec2 Math::projection(const Vec2& a, const Vec2& onto) {
	assert(onto.x != 0.f || onto.y != 0.f);

	float dp = dotProd(a, onto);
	return Vec2(
		(dp / (onto.x * onto.x + onto.y * onto.y)) * onto.x,	
		(dp / (onto.x * onto.x + onto.y * onto.y)) * onto.y	
	);
}
float Math::dotProd(const Vec2& a, const Vec2& b) {
	return (a.x * b.x) + (a.y * b.y);
}

float Math::dist(const sf::Vector2f& i, const sf::Vector2f& j) {
	sf::Vector2f d = i - j;
	return sqrtf(d.x * d.x + d.y * d.y);
};

float Math::dist(const Line& l) {
	sf::Vector2f d = l.end - l.start;
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
const sf::FloatRect Math::boundingBox(const Vec2& a, const Vec2& b) {
	sf::FloatRect r;
	r.left = std::min(a.x, b.x);
	r.top = std::min(a.y, b.y);
	r.width = std::max(a.x, b.x) - r.left;
	r.height = std::max(a.y, b.y) - r.top;
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
