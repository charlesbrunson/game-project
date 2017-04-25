#ifndef ENTITY_H
#define ENTITY_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
//#include "ResourceUser.hpp"
#include "AnimSprite.hpp"
#include "Globals.hpp"

#include <math.h>

// Generic entity class for gameplay
class Entity : public sf::Drawable {

public:
	
	// Probably unneccessary
	const bool canCreateEntities = false;

	Entity() {

	};

	virtual ~Entity() {

	}

	virtual void update(sf::Time deltaTime) = 0;
	
	// Velocity
	void setVelocity(sf::Vector2f vel);
	sf::Vector2f getVelocity();

	// Position
	virtual void setPosition(sf::Vector2f pos, bool updateOldBox = true) {
		position = pos;

		updateSpritePos(position);

	};

	void updateSpritePos(sf::Vector2f pos) {
		sprite.updateSpritePos(pos);
	}

	sf::Vector2f getPosition();

	// Sprite
	void setAnimSprite(AnimSprite spr);
	AnimSprite &getAnimSprite();

	bool toDelete = false;

	const bool isCollidable() { return hasCollidable; };

	int drawPriority = -1;
	bool drawUnderBackground() {
		return drawPriority >= 1000;
	};

	void setDrawPriority(int p) {
		changedDrawPriority = drawPriority != p;
		drawPriority = p;
	}

	bool changedDrawPriority = false;

protected:
	sf::Vector2f velocity;
	sf::Vector2f position;
	AnimSprite sprite;
	sf::Vector2f colOffset;

	bool hasCollidable = false;

private:
	
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
		target.draw(sprite, states);
	};

};

#endif