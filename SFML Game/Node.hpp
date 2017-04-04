#ifndef NODE_H
#define NODE_H

#include <SFML/System.hpp>
#include <vector>

//nodes represent objects and are used to populate levels

struct objNode {
	std::string name;

	std::string type;

	int x;
	int y;

	int width;
	int height;

	bool faceLeft;

	std::vector<std::pair<std::string, std::string>> args;

	bool isTemplate = false;
};

#endif