
#include <math.h>
#include <assert.h>

#include "util/Math.hpp"
#include "game/lvl/Tile.hpp"
#include "util/Log.hpp"

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

Vec2 Math::intersection(const Line& a, const Line& b) {

	sf::Vector2<double> a1;
	a1.x = a.start.x;
	a1.y = a.start.y;

	sf::Vector2<double> a2;
	a2.x = a.end.x;
	a2.y = a.end.y;

	sf::Vector2<double> b1;
	b1.x = b.start.x;
	b1.y = b.start.y;

	sf::Vector2<double> b2;
	b2.x = b.end.x;
	b2.y = b.end.y;

	bool aVert = a2.x - a1.x == 0.0;
	bool bVert = b2.x - b1.x == 0.0;

	if (!aVert && !bVert) {
		double aSlope = (a2.y - a1.y) / (a2.x - a1.x);
		double bSlope = (b2.y - b1.y) / (b2.x - b1.x);

		double aIntercept = (-aSlope * a2.x) + (a2.y);
		double bIntercept = (-bSlope * b2.x) + (b2.y);

		if (aSlope == bSlope) {
			Log::msg("Math::intersection, these are both parallel");
			return Vec2(0.f, 0.f);
		}

		sf::Vector2<double> intersection;
		intersection.x = (bIntercept - aIntercept) / (aSlope - bSlope);
		intersection.y = aSlope * intersection.x + aIntercept;

		return Vec2(intersection.x, intersection.y);
	}
	else if (aVert) {
		double bSlope = (b2.y - b1.y) / (b2.x - b1.x);
		double bIntercept = (-bSlope * b2.x) + (b2.y);

		return Vec2(a1.x, bSlope * a1.x + bIntercept);
	}
	else if (bVert) {
		double aSlope = (a2.y - a1.y) / (a2.x - a1.x);
		double aIntercept = (-aSlope * a2.x) + (a2.y);

		return Vec2(b1.x, aSlope * b1.x + aIntercept);
	}
	else {
		Log::msg("Math::intersection, these are both vertical");
		return Vec2(0.f, 0.f);
	}
};

Vec2 Math::projection(const Vec2& a, const Vec2& onto, bool ontoIsUnit) {

	if (onto.x == 0.f && onto.y == 0.f) {
		return Vec2(0.f, 0.f);
	}
	else if (onto.x == 0.f) {
		return Vec2(0.f, a.y);
	}
	else if (onto.y == 0.f) {
		return Vec2(a.x, 0.f);
	}

	float dp = dotProd(a, onto);
	if (ontoIsUnit) {
		return Vec2( dp * onto.x, dp  * onto.y);
	}
	else {
		return Vec2(
			( dp / (onto.x * onto.x + onto.y * onto.y) ) * onto.x,	
			( dp / (onto.x * onto.x + onto.y * onto.y) ) * onto.y	
		);
	}

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
float Math::normalAng(float rad) {
	float r = rad;
	if (r < 0.f) {
		r += 2.f * PI_F;
	}
	else if (r >= 2.f * PI_F) {
		r -= 2.f * PI_F;
	}
	return r;
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
