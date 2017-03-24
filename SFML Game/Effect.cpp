#include "Effect.hpp"


Effect::Effect(int type, bool flip, ResourceLoader *rloader) : Entity(rloader) {

	sprite.setResources(rloader);
	sprite.setHFlip(flip);

	switch (type) {
	default:
		toDelete = true;
		break;
	}
	sprite.setAnimation(anim);
	sprite.updateFrame();
};

// Custom constuctor
Effect::Effect(std::string texFile, Animation ani, int onLayer, bool flip, ResourceLoader *rloader) : Entity(rloader) {

	sprite.setResources(rMan);
	sprite.setHFlip(flip);

	sprite.getSprite()->setTexture(*rMan->getTexture(texFile));

	anim = ani;
	layer = onLayer;
	sprite.setAnimation(anim);
	sprite.updateFrame();
};

void Effect::update(sf::Time deltaTime) {

	//if sprite animation completes set to delete
	if (sprite.completedCurrentAnimation()) {

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