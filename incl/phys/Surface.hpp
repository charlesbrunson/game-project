#ifndef SURFACE_H
#define SURFACE_H

#include <SFML/Graphics.hpp>

#include "util/Math.hpp"


class Surface : public sf::Drawable {
public:
	Surface(Point s, Point e) : line(s, e) { Surface(Line(s,e)); };
	Surface(Line s) : line(s) { startConn = nullptr; endConn = nullptr; };

	std::string toString();

	Line line;

	Surface* startConn = nullptr;
	Surface* endConn = nullptr;

private:
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif
