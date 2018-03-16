#ifndef COLLIDABLE_H
#define COLLIDABLE_H

#include <SFML/Graphics.hpp>
#include <vector>

#include "obj/GameObject.hpp"

class Collidable {
public:

	struct Collider {
		sf::FloatRect box;
		sf::Vector2f offset;
	};

	enum Direction : int {
		UP,
		RIGHT,
		DOWN,
		LEFT
	};

	Collidable() {

	}
	Collidable(GameObject *p) {
		collidableParent = p;
	};

	void updateCollider(sf::Time deltaTime);
	bool isColliding(GameObject* obj);
	bool canCollide(int dir);
	bool addCollider(sf::FloatRect rect, sf::Vector2f off, int dir);
	bool addCollider(Collider col, int dir);

	std::vector<Collider>* getColliders(int dir);
	const sf::FloatRect getBoundingBox();

	bool isActive() const {
		return collidableActive;
	};

	void addCollidingObj(GameObject* obj, int dir) {
		collidingObjs.push_back(std::pair<GameObject*, int>(obj, dir));
	}

	GameObject* getParent() {
		return collidableParent;
	}
	void setParent(GameObject* p) {
		collidableParent = p;
	}

	void moveCollidingObj(GameObject *collidedObj, int dir, const sf::Time deltaTime);

	bool canCollideWith(GameObject *obj) {
		if (playerOnly && obj->objectType != GameObject::type::PLAYER) {
			return false;

		}
		else {
			return true;
		}

	}

	bool collidableActive = true;

protected:

	bool playerOnly = false;

	virtual void createColliders() = 0;
	
	GameObject *collidableParent = nullptr;

	std::vector<Collider> upColliders;
	std::vector<Collider> rightColliders;
	std::vector<Collider> downColliders;
	std::vector<Collider> leftColliders;
	sf::FloatRect boundingBox;


	std::vector<std::pair<GameObject*, int>> collidingObjs;
	
	void drawDebug(sf::RenderTarget &target, sf::RenderStates states) const {
#ifdef _DEBUG
		if (Gameplay_Globals::Debug::objectCollision && collidableActive){
			sf::RectangleShape r;
			
			//top - red
			r.setFillColor(sf::Color(255, 255, 255, 150));
			for (std::vector<Collider>::const_iterator it = upColliders.begin(); it != upColliders.end(); it++) {
				r.setPosition(sf::Vector2f(it->box.left, it->box.top));
				r.setSize(sf::Vector2f(it->box.width, it->box.height - 8));
				target.draw(r, states);
			}

			//right - green
			r.setFillColor(sf::Color(0, 255, 0, 150));
			for (std::vector<Collider>::const_iterator it = rightColliders.begin(); it != rightColliders.end(); it++) {
				r.setPosition(sf::Vector2f(it->box.left + 8, it->box.top));
				r.setSize(sf::Vector2f(it->box.width - 8, it->box.height));
				target.draw(r, states);
			}

			//left - blue
			r.setFillColor(sf::Color(0, 0, 255, 150));
			for (std::vector<Collider>::const_iterator it = leftColliders.begin(); it != leftColliders.end(); it++) {
				r.setPosition(sf::Vector2f(it->box.left, it->box.top));
				r.setSize(sf::Vector2f(it->box.width - 8, it->box.height));
				target.draw(r, states);
			}

			//down - yellow
			r.setFillColor(sf::Color(255, 0, 0, 150));
			for (std::vector<Collider>::const_iterator it = downColliders.begin(); it != downColliders.end(); it++) {
				r.setPosition(sf::Vector2f(it->box.left, it->box.top + 8));
				r.setSize(sf::Vector2f(it->box.width, it->box.height - 8));
				target.draw(r, states);
			}

			//bounding box
			r.setFillColor(sf::Color::Transparent);
			r.setOutlineThickness(1.f);
			r.setOutlineColor(sf::Color::White);
			r.setPosition(sf::Vector2f(boundingBox.left, boundingBox.top));
			r.setSize(sf::Vector2f(boundingBox.width, boundingBox.height));
			target.draw(r, states);
		}
#endif
	}

};

#endif
