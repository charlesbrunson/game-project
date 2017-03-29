#include "TestObject.hpp"
#include "PlayerRef.hpp"

Animation TestObject::anim_idle(sf::IntRect(0, 0, 16, 32), sf::Vector2f(8, 32), 1, sf::seconds(1.f));

TestObject::TestObject(ResourceLoader *rloader, Level *level, int ID) : GameObject(rloader, level, ID) {
	sprite.setAnimation(anim_idle);
	sprite.updateFrame();

	sprite.getSprite()->setTexture(*sprite.getResources()->getTexture("testenemy.png"));

	setCollisionSize(16, 32);

	drawPriority = 100;
	objectType = type::NEUTRAL;
	objectID = ID;

	collisionActive = true;
	isGlobal = false;
};

void TestObject::update(sf::Time deltaTime) {
	
	//apply gravity
	velocity.y += 300.f * deltaTime.asSeconds();
	setPosition(position += velocity * deltaTime.asSeconds());

	//face player
	sprite.setHFlip(position.x > gPlayerPos.x);

	resetCollisionFlags();
}

void TestObject::updateAnimation(sf::Time deltaTime) {
	sprite.update(deltaTime);
}