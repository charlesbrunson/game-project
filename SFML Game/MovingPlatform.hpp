#ifndef MOVINGPLATFORM_H
#define MOVINGPLATFORM_H

#include "GameObject.hpp"
#include "Collidable.hpp"

class MovingPlatform : public GameObject, public Collidable {
public:
	
	MovingPlatform(ResourceLoader *rloader, Level *level, int ID = -1) : GameObject(rloader, level, ID) {
		
		objectType = type::PLATFORM;
		collisionActive = false;

		drawPriority = 1000;

		waitTimer = sf::Time::Zero;
		travelTimer = sf::Time::Zero;
		travelDuration = sf::Time::Zero;

		pointCounter = 0;

		collidableParent = (GameObject*)this;
	};
	
	enum platformTypes : int {
		INVALID = -1,
		DEFAULT = 0

	};

	//set up platform based on given type
	bool setType(int t);

	//path control
	void appendPathPoint(sf::Vector2f point);
	bool insertPathPoint(sf::Vector2f point, int insert);
	void clearPath();
	sf::Vector2f incrementPath();
	sf::Vector2f getPreviousPoint();

	void update(sf::Time deltaTime);

	//std::vector<std::pair<GameObject*, int>> collidingObjs;

	sf::Time waitAtPoint = sf::seconds(1.f);
	//don't use anything higher than 1000 for your own good
	float platformSpeed = 80.f;

	const std::vector<sf::Vector2f> getPath() {
		return pathingPoints;
	};

	void readObjArgs() {
		int pathcount = 0;
		for (std::pair<sf::String, sf::String> arg : getObjectNode().args) {
			if (arg.first == "PLATTYPE") {
				setType(std::stoi((std::string)arg.second));
			}
			else if (arg.first == "SPEED") {
				platformSpeed = static_cast<float>(std::stoi((std::string)arg.second));
			}
			else if (arg.first == "WAIT") {
				waitAtPoint = sf::milliseconds(std::stoi((std::string)arg.second));
			}
			else if (arg.first == "PATHCOUNT") {
				pathcount = std::stoi((std::string)arg.second);
			}
		}

		if (pathcount > 0) {
			int count = 0;
			for (std::pair<sf::String, sf::String> arg : getObjectNode().args) {

				if (count >= pathcount)
					break;

				sf::String pathname = "PATH" + std::to_string(count);
				if (arg.first == pathname) {

					std::string s = arg.second;
					int space = s.find(' ');
					std::string x = s.substr(0, space);
					std::string y = s.substr(space + 1, s.size());

					appendPathPoint(sf::Vector2f((float)std::stoi(x), (float)std::stoi(y)));
					count++;

				}
			}
		}
	};


	const bool isCollidable() {
		return true;
	}
	void* getCollidable() {
		return (Collidable*)this;
	};

protected:
	int platformType;
	bool teleportOnLoop = false;

	int pointCounter = 0;
	std::vector<sf::Vector2f> pathingPoints;
	sf::Time waitTimer;

	sf::Vector2f targetPoint;
	bool targetInstantiated = false;

	bool hasPath() {
		return pathingPoints.size() > 0;
	}

	sf::Time travelDuration;
	sf::Time travelTimer;
	void move(sf::Time deltaTime);

	void createColliders() {
		//determine sizes of each collision box
		Collider upCol;
		Collider rightCol;
		Collider downCol;
		Collider leftCol;

		upCol.box.height = std::min<float>(collisionBox.height, (float)tileSpacing);
		downCol.box.height = std::min<float>(collisionBox.height, (float)tileSpacing);

		upCol.box.width = collisionBox.width;
		downCol.box.width = collisionBox.width;

		rightCol.box.width = std::min<float>(collisionBox.width, (float)tileSpacing);
		leftCol.box.width = std::min<float>(collisionBox.width, (float)tileSpacing);

		rightCol.box.height = collisionBox.height;
		leftCol.box.height = collisionBox.height;

		upCol.offset = sf::Vector2f(-collisionBox.width / 2, -collisionBox.height);
		leftCol.offset = sf::Vector2f(-collisionBox.width / 2, -collisionBox.height);
		rightCol.offset = sf::Vector2f(-rightCol.box.width + collisionBox.width / 2, -collisionBox.height);
		downCol.offset = sf::Vector2f(-collisionBox.width / 2, -downCol.box.height);


		addCollider(upCol, Collidable::UP);
		addCollider(rightCol, Collidable::RIGHT);
		addCollider(downCol, Collidable::DOWN);
		addCollider(leftCol, Collidable::LEFT);
	};

	void draw(sf::RenderTarget &target, sf::RenderStates states) const {
		GameObject::draw(target, states);
#ifdef _DEBUG
		if (Gameplay_Globals::Debug::platformPathing && velocity != sf::Vector2f(0, 0)) {

			sf::RectangleShape pathTarget;

			pathTarget.setPosition(targetPoint);
			pathTarget.setSize(sf::Vector2f(1.f, 1.f));
			pathTarget.setFillColor(sf::Color(0, 0, 255, 255));

			target.draw(pathTarget, states);
		}
#endif

		drawDebug(target, states);

	};

	Animation anim;
};
#endif