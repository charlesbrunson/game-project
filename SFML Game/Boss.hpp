#ifndef BOSS_H
#define BOSS_H

#include "Enemy.hpp"

class Boss : public Enemy {
public:
	Boss(Level *l);

	void setArena(sf::FloatRect area);

	void update(sf::Time deltaTime);

	virtual void die(sf::Time deltaTime);

	void kill();

	bool isDying();
	
	bool entranceComplete = false;

protected:
	sf::FloatRect arenaArea;
	sf::Time deathDuration;
	sf::Time deathTimer;

	sf::Vector2f deathPos;

	bool dying = false;

};

#endif