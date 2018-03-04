#include "Enemy.hpp"

#include "PlayerRef.hpp"
#include "Collidable.hpp"
#include "CamRef.hpp"

void Enemy::despawn() {
	if (!toDelete && removeOnDespawn && spawnCounter > 0) {
		toDelete = true;
		if (health <= 0) {
			kill();
		}
	}
	else {
		asleep = true;
		if (isCollidable()) {
			((Collidable*)this)->collidableActive = false;
		}
		setPosition(spawnPos);
		sprite.setHFlip(spawnFlipped);
	}
};

void Enemy::spawn() {
	spawnCounter++;
	asleep = false;
	attackable = true;
	init();
	setPosition(spawnPos);
	sprite.setHFlip(spawnFlipped);
	if (facePlayerOnSpawn) {
		facePlayer();
	}
	if (isCollidable()) {
		((Collidable*)this)->collidableActive = true;
	}
};

//spawn if were within camera view
void Enemy::forceSpawn() {
	if (isVisible()) {
		spawn();
		onScreen = true;
	}
};

void Enemy::tryWake() {
	//spawn when entering player view, wake
	//construct floatrect of view + margins
	sf::FloatRect activeArea = (sf::FloatRect)lvl->levelArea;

	activeArea.left = gCameraPos.x - ((float)GAMEWIDTH / 2) - (float)spawnMargin;
	activeArea.top = gCameraPos.y - ((float)GAMEHEIGHT / 2) - (float)spawnMargin;
	activeArea.width = (float)GAMEWIDTH + (2 * (float)spawnMargin);
	activeArea.height = (float)GAMEHEIGHT + (2 * (float)spawnMargin);

	bool containsActive = activeArea.intersects(collisionBox);
	//bool containsCam = isVisible();
	
	if (asleep && !onScreen && containsActive) {
		spawn();
		return;
	}

	else if (!asleep && onScreen && !containsActive && despawnOnExitCamera) {
		despawn();
		return;
	}
	onScreen = containsActive;
};

void Enemy::setSpawn() {
	spawnPos = position;
	spawnFlipped = sprite.getHFlip();
	forceSpawn();
	initSpawn = true;
};

void Enemy::update(sf::Time deltaTime) {

	if (tryForceSpawn) {
		tryForceSpawn = false;
		forceSpawn();
	}
	if (active) {
		if (!initSpawn) {
			setSpawn();
		}

		tryWake();
		if (!asleep) {

			//run act() if we're awake and not stunned
			if (stun == sf::Time::Zero) {
				act(deltaTime);
			}

			else {
				if (stun < deltaTime) {
					sf::Time newDelta = deltaTime - stun;
					stun = sf::Time::Zero;
					update(newDelta);

				}
				else {
					//do nothing
					stun -= deltaTime;

					//probably unneccessary
					if (stun < sf::Time::Zero) {
						stun = sf::Time::Zero;
					}
				}
			}
		}
	}
};

bool Enemy::isSleeping() const {
	return asleep;
};

bool Enemy::hurt(int amount, sf::Time stunTime = sf::seconds(0)) {
	if (!asleep) {
		health -= amount;
		if (!stunImmune)
			stun = stunTime;

		if (health <= 0) {
			kill();
		}
		else {
			//createSound(Resources::ENEMY_HURT);
		}
		return true;
	}
	return false;
};

void Enemy::kill() {
	//createSound(Resources::ENEMY_DEATH);
	createDeathFX(sf::Vector2f(0, -9.f));

	if (isCollidable()) {
		((Collidable*)this)->collidableActive = false;
	}

	attackable = false;

	addScore(killValue);

	attackBox.setActive(false);
	stun = sf::Time::Zero;

	if (!respawning)
		toDelete = true;
	else
		despawn();
};

void Enemy::facePlayer() {

	if (gPlayerPos.x < position.x) {
		sprite.setHFlip(true);
	}
	else if (gPlayerPos.x > position.x) {
		sprite.setHFlip(false);
	}

};

bool Enemy::isVisible() {
	return getCameraArea().intersects(collisionBox);
};
