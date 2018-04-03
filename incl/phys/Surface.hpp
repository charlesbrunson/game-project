#ifndef SURFACE_H
#define SURFACE_H

#include <SFML/Graphics.hpp>

#include "util/Math.hpp"


class Surface : public sf::Drawable {
public:
	Surface(Point s, Point e) : line(s, e) { };
	Surface(Line s) : line(s) { };

	std::string toString() {
		return "line:(" +
			std::to_string(line.start.x) + ", " + std::to_string(line.start.y) +
			") -> (" +
			std::to_string(line.end.x) + ", " + std::to_string(line.end.y) + ")";
	}

	Line line;

private:
	void draw(sf::RenderTarget &target, sf::RenderStates states) const {
#ifdef _DEBUG
		sf::VertexArray lineVert(sf::Lines, 2);
		lineVert[0] = sf::Vertex(line.start, sf::Color::Green);
		lineVert[1] = sf::Vertex(line.end, sf::Color::Red);
		target.draw(lineVert, states);
#endif
	};

};

#endif
