#ifndef GATE_H
#define GATE_H

#include "GameObject.hpp"
#include "Collidable.hpp"

class Gate : public GameObject, public Collidable {
public:
	Gate(ResourceLoader *rloader, Level *level, int ID = -1) : GameObject(rloader, level, ID) {

		attackable = false;

		objectType = GameObject::type::NEUTRAL;
		drawPriority = 1000;

		collidableParent = (GameObject*)this;
		
	};
	
	void setHeight(int h);

	void readObjArgs();
	
	void setOpen(bool open = true);

	void update(sf::Time deltaTime);

	const bool isCollidable() {
		return true;
	}
	void* getCollidable() {
		return (Collidable*)this;
	};

	bool isOpen() {
		return currentState == OPEN;
	}

protected:
	enum state : int {
		OPEN,
		CLOSED
	};

	enum transDir : int {
		NONE,
		LEFT,
		RIGHT
	};
	int transitionOnTouch = NONE;

	const sf::Time gateMoveTime = sf::seconds(0.2f);
	sf::Time animTimer = gateMoveTime;

	bool hasColliders = false;

	int currentState = CLOSED;

	int segmentCount;
	sf::Sprite gateEnd;
	std::vector<sf::Sprite> gateSegments;

	void createColliders();

	void moveGateSegments(bool opening);

	void draw(sf::RenderTarget &target, sf::RenderStates states) const {

		if (currentState == CLOSED || (currentState == OPEN && animTimer < gateMoveTime)) {
			target.draw(gateEnd, states);
			for (std::vector<sf::Sprite>::const_iterator it = gateSegments.begin(); it != gateSegments.end(); it++) {
				target.draw(*it, states);
			}
		}

		GameObject::draw(target, states);

		drawDebug(target, states);

	};

};

#endif