#include "Boss.hpp"
//#include "Coin.hpp"

#include "PlayerRef.hpp"
#include "AlertSystem.hpp"

Boss::Boss(Level *l) : Enemy(l) {
	respawning = false;
	removeOnDespawn = true;
	despawnOnExitCamera = false;
	stunImmune = true;

	deathDuration = sf::seconds(3.f);
	deathTimer = sf::Time::Zero;
};

void Boss::setArena(sf::FloatRect area) {
	arenaArea = area;
};

void Boss::update(sf::Time deltaTime) {
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

			// Run act() if we're awake and not stunned
			if (stun == sf::Time::Zero) {
				if (dying)
					die(deltaTime);
				else
					act(deltaTime);
			}

			else {
				if (stun < deltaTime) {
					sf::Time newDelta = deltaTime - stun;
					stun = sf::Time::Zero;
					update(newDelta);

				}
				else {
					// Do nothing
					stun -= deltaTime;

					// Probably unneccessary
					if (stun < sf::Time::Zero) {
						stun = sf::Time::Zero;
					}
				}
			}
		}
	}
};


void Boss::die(sf::Time deltaTime) {

	float fxPerSec = 8.f;
	sf::Time nextEffect = sf::seconds(ceil(deathTimer.asSeconds() / (1.f / fxPerSec)) * (1.f / fxPerSec));

	if (deathTimer < nextEffect && deathTimer + deltaTime >= nextEffect) {
		// Dynamic death animation

		sf::Vector2f pos(getCollision().left, getCollision().top);

		pos.x += getCollision().width * 0.1f;
		pos.y += getCollision().height * 0.1f;

		pos.x += rand() % ((int)(getCollision().width * 0.8f) + 1);
		pos.y += rand() % ((int)(getCollision().height * 0.8f) + 1);

		//Effect *fx = new Effect(Effect::EffectType::EXPLOSION, false);
		Effect *fx = new Effect();
		fx->setPosition(pos);
		createEffect(fx);
		
		int rPosRange = 2;
		sf::Vector2f r((rand() % (rPosRange * 2)) - rPosRange, (rand() % (rPosRange * 2)) - rPosRange);
		setPosition(deathPos + r);

		//createSound(Resources::ENEMY_DEATH);
	}

	deathTimer += deltaTime;
	if (deathTimer >= deathDuration) {
		// Finish dying

		sf::Vector2f center(collisionBox.left + collisionBox.width / 2, collisionBox.top + collisionBox.height / 2);

		Effect *fx = new Effect();
		fx->setPosition(center);
		createEffect(fx);


		attackBox.setActive(false);
		stun = sf::Time::Zero;

		toDelete = true;
		//createSound(Resources::ENEMY_HURT);
	}
}

void Boss::kill() {
	touchDamage = 0;

	if (!dying) {
		//createSound(Resources::BOSS_DEATH);

		AlertSystem::Alert a(AlertSystem::Message::PAUSE, 1000);
		AlertSystem::pushAlert(a);

		AlertSystem::Alert b(AlertSystem::Message::CAM_SHAKE, 1000, sf::Vector2f((gPlayerPos.x < position.x ? 1 : -1) * 4.f, 0.f));
		AlertSystem::pushAlert(b);
	}

	deathPos = position;
	dying = true;
	attackable = false;

	setVelocity(sf::Vector2f());
	collisionActive = false;
}

bool Boss::isDying() {
	return dying;
}
