#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "GameObject.hpp"
#include "Collidable.hpp"
//#include "MovingPlatform.hpp"
#include "Effect.hpp"
#include "Level.hpp"
#include "Node.hpp"

#include "Player.hpp"

#include <memory>


//manages objects in gameplay state
class ObjectManager {
public:

	struct Collision {
		bool horizontal = true;
		float magnitude = 0;
		sf::FloatRect rect;
		sf::Vector2f velocity = sf::Vector2f(0.f, 0.f);

		Collidable *parent = nullptr;

		bool valid = true;
		int dir = -1; //0 = up, 1 = right, etc
	};

	ObjectManager(Level *lvl) {
		gameLevel = lvl;
	};

	~ObjectManager() {
		clear();
	}
	
	void clear();
	void clearExceptPlayer();
	void clearExceptGlobals();
	void clearObjectsNotInLevel();
	void update(sf::Time deltaTime, int *score, bool onlyPlayer = false);
	void insertObject(GameObject* obj);

	//initializing obj lists
	void readObjNodes(std::vector<objNode> &objList, Level* lvl, 
		bool skipPlayer = false, sf::Vector2f offsetPos = sf::Vector2f());
		
	Player* plr = nullptr;

	Player* getPlayer() const;

	GameObject* getObjectByName(std::string name);
	
	Level *gameLevel;
	
	std::vector<std::unique_ptr<GameObject>>* getObjects();
	std::vector<std::unique_ptr<Effect>>* getEffects();
	std::vector<std::unique_ptr<GameObject>>* getTriggers();

	std::vector<objNode>* getObjectNodes();
	std::vector<objNode> nodes;

private:

	//a vector of vector iterators to unique ptrs of gameobjects, yep
	std::vector<std::vector<std::unique_ptr<GameObject>>::const_iterator> toRemove;

	std::vector<std::unique_ptr<GameObject>> objects;
	std::map<GameObject* ,Collidable*> collidables;
	std::vector<std::unique_ptr<Effect>> effects;
	std::vector<std::unique_ptr<GameObject>> triggers;
	
	//collision
	void doCollision(GameObject* obj) {

		std::vector<Collision> collisions;

		if (obj->objectType == GameObject::type::PLAYER) {
			applyLevelTriggers(obj);
		}

		for (std::pair<GameObject*, Collidable*> collidable : collidables) {
			if (collidable.second->isActive() && collidable.second->canCollideWith(obj) && obj != collidable.second->getParent() 
				&& obj->getCollision().intersects(collidable.second->getBoundingBox())) {
				doCollidableCollision(collidable.second, obj, &collisions);
			}
		}
		doLevelCollision(obj, &collisions);
		validateCollisions(&collisions);
		resolveCollisions(obj, &collisions);

	}
	void doLevelCollision(GameObject *obj, std::vector<Collision> *collisions);
	void doCollidableCollision(Collidable *col, GameObject *obj, std::vector<Collision> *collisions);
	void validateCollisions(std::vector<Collision> *collisions);
	void resolveCollisions(GameObject *obj, std::vector<Collision> *collisions);

	std::map<std::string, sf::Sound> sounds;

	struct Sound {
		std::string sound = "";
		sf::Time timeoutTimer = sf::Time::Zero;
	};
	std::vector<Sound> soundsToPlay;

	std::vector<GameObject*> addObjects;

	void updateObjects(sf::Time deltaTime, int *score);
	void updateEffects(sf::Time deltaTime);

	void applyLevelTriggers(GameObject *obj);

};

#endif