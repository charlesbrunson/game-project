#ifndef NODE_H
#define NODE_H

#include <SFML/System.hpp>
#include <vector>

//nodes represent objects and are used to populate levels

struct objNode {
	sf::String name;

	sf::String type;

	int x;
	int y;

	int width;
	int height;

	bool faceLeft;

	std::vector<std::pair<sf::String, sf::String>> args;

	bool isTemplate = false;
};

#endif