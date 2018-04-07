#ifndef SURFACE_H
#define SURFACE_H

#include <SFML/Graphics.hpp>

#include "util/Math.hpp"


class Surface : public sf::Drawable {
public:
	Surface(Point s, Point e) : line(s, e) { };
	Surface(Line s) : line(s) { };

	std::string toString();

	Line line;

private:
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif
