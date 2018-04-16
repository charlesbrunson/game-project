#ifndef MATH_H
#define MATH_H

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "GridVector.hpp"
#include <math.h>

extern const float PI_F;

enum Cardinal {
	NO_DIR = -1,
	NORTH = 0,
	EAST,
	SOUTH,
	WEST,
	DIR_COUNT
};

typedef sf::Vector2f Vec2;
typedef GridVector GridVec2;
typedef sf::Vector2f Point;
typedef sf::FloatRect Rect;

template<typename Value>
using GridMap = std::map<GridVec2, Value>;

struct Line;

namespace Math {
	const sf::Vector2f diff(const sf::Vector2f& i, const sf::Vector2f& j);

	bool intersects(const Line& line, const sf::FloatRect& rect, bool lineIsSegment = true);

	float pointOnLine(const Line& line, const Point& p);

	Vec2 projection(const Vec2& a, const Vec2& onto, bool ontoIsUnit = false);
	float dotProd(const Vec2& a, const Vec2& b);

	float dist(const sf::Vector2f& i, const sf::Vector2f& j);
	float dist(const Line& l);

	float magnitude(const sf::Vector2f& v);

	float lerp(float a, float b, float perc);
	const sf::Vector2f lerp(sf::Vector2f a, sf::Vector2f b, float perc);

	const sf::Vector2f normalize(const sf::Vector2f& v);
	const sf::Vector2f rotate(const sf::Vector2f& v, float rad);

	float angle(const sf::Vector2f& v);
	float angle(const sf::Vector2f& from, const sf::Vector2f& to);
	float normalAng(float rad);

	const sf::FloatRect boundingBox(const sf::FloatRect& a, const sf::FloatRect& b);
	const sf::FloatRect boundingBox(const Vec2& a, const Vec2& b);
	const sf::FloatRect lerpBox(const sf::FloatRect& a, const sf::FloatRect& b, float perc = 0.5f);
	const sf::IntRect gridBounds(const sf::FloatRect& a);

	const sf::Vector2f size(const sf::FloatRect& a);
	const sf::Vector2f topleft(const sf::FloatRect& a);
	const sf::Vector2f topright(const sf::FloatRect& a);
	const sf::Vector2f bottomleft(const sf::FloatRect& a);
	const sf::Vector2f bottomright(const sf::FloatRect& a);
	const sf::Vector2f center(const sf::FloatRect& a);

	float toDeg(float rad);
	float toRad(float deg);
};

struct Line {
	Line(Point s, Point e) : start(s), end(e) {};

	Point start;
	Point end;

	Vec2 getVector() {
		return end - start;
	};
	Line reverse() {
		return Line(end, start);
	};
	bool operator== (const Line &l) {
		return l.start == start && l.end == end;
	}
	bool operator!= (const Line &l) {
		return l.start != start || l.end != end;
	}
	bool hasPoint(Point p) const {
		return p.x >= std::min(start.x, end.x) && p.x <= std::min(start.x, end.x) &&
			Math::pointOnLine(*this, p) == 0.f;
	}

	Vec2 getUnit() const {
		return (end - start) / Math::dist(start, end);
	}
	Vec2 getLeftHandUnit() const {
		Vec2 r = getUnit();
		return Vec2(r.y, -r.x);
	}

	bool isHorizontal() const {
		return start.y == end.y;
	};
	bool isVertical() const {
		return start.x == end.x;
	};
};

struct LineIntersection {

	enum LineIntersectionType {
		NO_INTERSECT,
		COLLINEAR,
		SINGLE_INTERSECT
	};

	LineIntersectionType intersectType = NO_INTERSECT;
	Line intersectLine;
	Point intersectPoint;

};

#endif
