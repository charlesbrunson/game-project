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
	lineVert[0] = sf::Vertex(line.start, sf::Color::Green);
	lineVert[1] = sf::Vertex(line.end,   sf::Color::Red);
	target.draw(lineVert, states);

	if (startConn == nullptr) {
		sf::RectangleShape t;
		t.setSize(sf::Vector2f(1.f, 1.f));
		t.setPosition(line.start + Vec2(-0.5f, -0.5f));
		t.setFillColor(sf::Color::Red);
		target.draw(t, states);
	}
	if (endConn == nullptr) {
		sf::RectangleShape t;
		t.setSize(sf::Vector2f(1.f, 1.f));
		t.setPosition(line.end + Vec2(-0.5f, -0.5f));
		t.setFillColor(sf::Color::Red);
		target.draw(t, states);
	}
#endif
};

