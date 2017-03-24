#ifndef GROUNDENEMY_H
#define GROUNDENEMY_H

#include "Enemy.hpp"

class GroundEnemy : public Enemy {
public:

	GroundEnemy(ResourceLoader *rloader, Level *l, int ID = -1) : Enemy(rloader, l, ID) {

	}
	

protected:
	int surfaceLeft = 0;
	int surfaceRight = 0;
	int surfaceHeight = 0;

	bool checkedLeft = false;
	bool checkedRight = false;

	void setSpawn();

	bool checkEdge(bool LeftElseRight);

	virtual void act(sf::Time deltaTime) = 0;

	//bool for if move was successful
	bool snapToSurface();
	
};

#endif