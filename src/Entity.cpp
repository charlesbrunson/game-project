#include "obj/Entity.hpp"

void Entity::setVelocity(sf::Vector2f vel) {
	velocity = vel;
};
sf::Vector2f Entity::getVelocity() {
	return velocity;
};


sf::Vector2f Entity::getPosition() {
	return position;
};

void Entity::setAnimSprite(AnimSprite spr) {
	sprite = spr;
};
AnimSprite &Entity::getAnimSprite() {
	return sprite;
};
