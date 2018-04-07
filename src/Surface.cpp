#include "phys/Surface.hpp"

std::string Surface::toString() {
	return "line:(" +
		std::to_string(line.start.x) + ", " + std::to_string(line.start.y) +
		") -> (" +
		std::to_string(line.end.x) + ", " + std::to_string(line.end.y) + ")";
}

void Surface::draw(sf::RenderTarget &target, sf::RenderStates states) const {
#ifdef _DEBUG
	sf::VertexArray lineVert(sf::Lines, 2);
	lineVert[0] = sf::Vertex(line.start, sf::Color::Red);
	lineVert[1] = sf::Vertex(line.end,   sf::Color::Red);
	if (line.isHorizontal() || line.isVertical()) {
		lineVert[0].color = sf::Color::Green;
		lineVert[1].color = sf::Color::Green;
	}
	target.draw(lineVert, states);
#endif
};

