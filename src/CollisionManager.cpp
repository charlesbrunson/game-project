#include "CollisionManager.hpp"
#include <vector>


CollisionManager::CollisionManager() {
	for (int i = 0; i < Static::CollisionType::COUNT; i++) {
		staticsByType.insert(std::make_pair((Static::CollisionType)i, std::vector<Static*>()));
	}
}

CollisionManager::CollisionManager(sf::Vector2i staticSize) : CollisionManager() {
	for (int x = 0; x < staticSize.x; x++) {
		for (int y = 0; y < staticSize.y; y++) {
			staticsByGrid.insert(std::make_pair(GridVector(x,y), std::vector<Static*>()));
		}
	}
}

CollisionManager::CollisionManager(Level* lvl) : CollisionManager(sf::Vector2i(lvl->levelArea.width, lvl->levelArea.height)) {
	setWorldCollision(lvl);
}

void CollisionManager::update(sf::Time delta) {
	for (auto solid = solids.begin(); solid != solids.end(); solid++) {
		collide(&*solid);		
	}
}

void CollisionManager::setWorldCollision(Level* lvl) {

}

Static* CollisionManager::createStatic(sf::FloatRect area, Static::CollisionType colType) {
	statics.push_back(Static());
	statics.back().setArea(area);
	statics.back().resetPrevArea();
	statics.back().type = colType;

	staticsByType.at(colType).push_back(&statics.back());

	sf::IntRect gridArea = Math::gridBounds(area);
	GridVector pos;
	for (int x = gridArea.left; x < gridArea.left + gridArea.width; x++) {
		for (int y = gridArea.left; y < gridArea.left + gridArea.width; y++) {
			pos.x = x;
			pos.y = y;
			staticsByGrid.at(pos).push_back(&statics.back());
		}
	}

	return &statics.back();
}

Solid* CollisionManager::createSolid(sf::FloatRect area) {
	solids.push_back(Solid());
	solids.back().setArea(area);
	solids.back().resetPrevArea();
	return &solids.back();
}

Static* CollisionManager::removeStatic(Static* statObj) {
	for (auto i = statics.begin(); i != statics.end(); i++) {
		if (&*i == statObj) {
			statics.erase(i);
			//successful
			return nullptr;
		}
	}
	return statObj;
}

Solid* CollisionManager::removeSolid(Solid* solid) {
	for (auto i = solids.begin(); i != solids.end(); i++) {
		if (&*i == solid) {
			solids.erase(i);
			//successful
			return nullptr;
		}
	}
	return solid;
}

void CollisionManager::collide(Solid* solid) {

		std::vector<Collision> collisions;

		collideWithStatic(solid, &collisions);
		validateCollisions(&collisions);
		resolveCollisions(solid, &collisions);
}

void CollisionManager::collideWithStatic(Solid* solid, std::vector<Collision> *collisions) {
	const sf::FloatRect c = solid->getArea();
	const sf::FloatRect oldC = solid->getPrevArea();

	//create bounding box for object's current frame collision box and previous frame collision box
	const sf::FloatRect bb(Math::boundingBox(c, oldC));

	auto makeCol = [solid](Static *st, sf::Vector2f solveOffset) -> Collision {
		Collision col;
		col.valid = true;
		col.stat = st;
		col.solid = solid;
		col.initial = Math::center(solid->getArea());
		col.offset = solveOffset;
		return col;
	};

	std::vector<Static*> *right   = &staticsByType.at(Static::CollisionType::RIGHTWALL);
	std::vector<Static*> *left    = &staticsByType.at(Static::CollisionType::LEFTWALL);
	std::vector<Static*> *floors  = &staticsByType.at(Static::CollisionType::FLOOR);
	std::vector<Static*> *ceiling = &staticsByType.at(Static::CollisionType::CEILING);

	sf::FloatRect area;
	//solid going left
	if (c.left < oldC.left && !solid->ignoreDir[Cardinal::EAST]) {
		for (auto i = right->cbegin(); i != right->cend(); i++) {
			area = (*i)->getArea();
			if (area.intersects(bb) && bb.left <= area.left + area.width && oldC.left >= area.left + area.width) {
				collisions->push_back(makeCol(*i, sf::Vector2f(area.left + area.width - c.left, 0.f)));
			}
		}
	}
	//solid going right
	else if (c.left + c.width > oldC.left + oldC.width && !solid->ignoreDir[Cardinal::WEST]) {	
		for (auto i = left->cbegin(); i != left->cend(); i++) {
			area = (*i)->getArea();
			if (area.intersects(bb) && bb.left + bb.width >= area.left && oldC.left + oldC.width <= area.left) {
				collisions->push_back(makeCol(*i, sf::Vector2f(area.left - c.left - c.width, 0.f)));
			}
		}
	}
	//solid going down
	if (c.top + c.height > oldC.top + oldC.height && !solid->ignoreDir[Cardinal::NORTH]) {
		for (auto i = floors->cbegin(); i != floors->cend(); i++) {
			area = (*i)->getArea();
			if (area.intersects(bb) && bb.top + bb.height >= area.top && oldC.top + oldC.height <= area.top) {
				collisions->push_back(makeCol(*i, sf::Vector2f(0.f, area.top - c.top - c.height)));
			}
		}
	}
	//solid going up
	else if (c.top < oldC.top && !solid->ignoreDir[Cardinal::SOUTH]) {
		for (auto i = ceiling->cbegin(); i != ceiling->cend(); i++) {
			area = (*i)->getArea();
			if (area.intersects(bb) && bb.top <= area.top + area.height && oldC.top >= area.top + area.height) {
				collisions->push_back(makeCol(*i, sf::Vector2f( 0.f, area.top + area.height - c.top)));
			}
		}
	}
}

void CollisionManager::validateCollisions(std::vector<Collision> *collisions) {

}

void CollisionManager::resolveCollisions(Solid* solid, std::vector<Collision> *collisions) {

}

