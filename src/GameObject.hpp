#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "Effect.hpp"
#include "Entity.hpp"
#include "Level.hpp"
#include "Node.hpp"
#include "TemplateUser.hpp"

class GameObject : public Entity {
public:

	std::string name;

	GameObject(Level *level) : Entity() {
		lvl = level;
		//objectID = ID;
		collisionBox = sf::FloatRect();
	};

	virtual void interact(GameObject* obj) {};

	virtual void update(sf::Time deltaTime) {};
	virtual void updateAnimation(sf::Time deltaTime) {};

	virtual bool hasObjectNode() { return hasNode; }

	void setObjectNode(objNode &o) {
		node.name = o.name;
		node.type = o.type;

		node.faceLeft = o.faceLeft;
		node.x = o.x;
		node.y = o.y;
		node.width = o.width;
		node.height = o.height;

		if (o.args.size() > 0) {
			node.args = o.args;
			readObjArgs();
		}

		hasNode = true;
	};

	virtual objNode &getObjectNode() { return node; };

	inline const bool getGrounded(){ return onGround; };
	void setGrounded(bool i){ onGround = i; };
	
	// Collision
	void setCollision(sf::FloatRect col);
	sf::FloatRect getCollision();
	
	void setPosition(sf::Vector2f pos, bool updateOldBox = true);

	//object types
	enum type : int {
		UNKNOWN = -1,
		PLATFORM,
		PLAYER,
		ENEMY,
			PROJECTILE_ENEMY,
			BOSS,
		ENEMY_END,
		NEUTRAL,
		PROJECTILE_PLAYER,
	};

	//only enemies should use stuntime
	virtual bool hurt(int amount, sf::Time stunTime = sf::seconds(0)) { return false; };
	virtual void kill() { toDelete = true; };

	//-1 == infinite/undefined
	int health = -1;
	int maxHealth = -1;

	int touchDamage = 0;

	int objectID = -1;
	int objectType = type::UNKNOWN;

	bool collisionActive = false;

	void setLevel(Level *level);
	Level * getLevel();

	std::vector<GameObject*> createdObjs;
	std::vector<Effect*> createdEffects;
	std::vector<std::string> createdSounds;

	void createObject(GameObject* obj);
	void createEffect(Effect* fx);
	void createSound(std::string soundFile);

	inline const sf::FloatRect getPrevFrameCollision() { return oldBox; };
	inline const bool isDroppingThroughFloor() { return droppingThroughFloor; };

	inline bool isAttackable() { return attackable; };
	inline bool isPlayerAttackableOnly() { return playerAttackOnly; };

	virtual void readObjArgs() {};

	virtual void* getCollidable() {	return nullptr;	};

	bool canDropThroughFloor();
	bool droppingThroughFloor = false;

	bool collisionUp = false;
	bool collisionRight = false;
	bool collisionDown = false;
	bool collisionLeft = false;

	inline bool isEnemy() { return objectType >= ENEMY && objectType <= ENEMY_END; };
	inline bool isNeutral() { return objectType == NEUTRAL; };
	inline bool isProjectile() { return objectType == PROJECTILE_ENEMY || objectType == PROJECTILE_PLAYER; };
	inline bool isGlobalObject() { return isGlobal; };

	bool isTemplateUser() { return getTemplateUser() != nullptr; };
	virtual void* getTemplateUser() { return nullptr; };

	int scoreToAdd = 0;

	sf::Vector2f camFollowOffset;

protected:

	//object will be carried through level transitions
	bool isGlobal = false;

	void addScore(int num) {scoreToAdd += num;}

	//pointer to level we're in
	Level *lvl;

	//objNode we're create by
	objNode node;
	bool hasNode = false;

	//object's collision bounds
	sf::FloatRect collisionBox;
	sf::FloatRect oldBox;

	//when this is true this object is due to be removed next step
	bool onGround = false;

	//remember to add deltatime to this every step if you're going to use it
	sf::Time lifeTime;
	
	//enable flashing effect
	bool sfxFlashing = false;
	//enable white effect
	bool sfxWhite = false;

	bool attackable = true;
	bool playerAttackOnly = false;
	
	void setCollisionSize(float width, float height, bool centerOrigin = false);

	void setCollisionPosition(sf::Vector2f p);

	sf::Vector2f getCollisionPosition();

	void resetCollisionFlags();

	bool doNotDraw = false;

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};


#endif