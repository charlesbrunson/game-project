#include "Effect.hpp"


Effect::Effect(int type, bool flip) {

	//depreciated, dont use this

	//sprite.setResources(rloader);
	sprite.setHFlip(flip);

	/*
	switch (type) {
	default:
		toDelete = true;
		break;
	}
	*/
	toDelete = true;

	//sprite.setAnimation(anim);
	sprite.updateFrame();
};

// Custom constuctor
Effect::Effect(std::string texFile, std::string ani, int onLayer, bool flip) {

	//sprite.setResources(rMan);
	sprite.setHFlip(flip);

	sprite.setTexFile(RL()->getTexFile(texFile));

	//anim = ani;
	layer = onLayer;
	sprite.setAnimation(ani);
	sprite.updateFrame();
};

void Effect::update(sf::Time deltaTime) {

	//if sprite animation completes set to delete
	if (sprite.isComplete()) {

		toDelete = true;
	}
	else {

		//update sprite
		sprite.update(deltaTime);
		sprite.updateFrame();


		if (parent == nullptr) {
			setPosition(position + (velocity * deltaTime.asSeconds()));
		}
		else {
			if (parent->toDelete) {
				toDelete = true;
			}
			else {
				sprite.setHFlip(parent->getAnimSprite().getHFlip());
				setPosition(parent->getPosition() + sf::Vector2f((parent->getAnimSprite().getHFlip() ? -1 : 1) * parentOffset.x, parentOffset.y));
			}
		}

	}

};

void Effect::setParent(Entity *p, sf::Vector2f off) {
	parent = p;
	parentOffset = off;

	sprite.setHFlip(parent->getAnimSprite().getHFlip());
	setPosition(parent->getPosition() + sf::Vector2f((parent->getAnimSprite().getHFlip() ? -1 : 1) * parentOffset.x, parentOffset.y));
};

Entity* Effect::getParent() {
	return parent;
};