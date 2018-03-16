#include "obj/TestObject.hpp"
#include "obj/plr/PlayerRef.hpp"

//Animation TestObject::anim_idle(sf::IntRect(0, 0, 16, 32), sf::Vector2f(8, 32), 1, sf::seconds(1.f));

TestObject::TestObject(Level *level) : GameObject(level) {

	sprite.setTexFile(RL()->getTexFile("sprites/testenemy.png"));
	sprite.setAnimation("idle");

	setCollisionSize(16, 32);

	drawPriority = 100;
	objectType = type::NEUTRAL;

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
