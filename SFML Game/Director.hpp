#ifndef DIRECTOR_H
#define DIRECTOR_H

#include "ObjectManager.hpp"
#include "Zone.hpp"
//#include "MusicPlayer.hpp"

// Manages level events, basically a level script
// Template for other Directors
class Director {
public:
	Director(Zone *z, ObjectManager *o, GameCamera *c);
	void update(sf::Time deltaTime, bool gamePaused = false);

	// Director action to be implemented
	virtual void act(sf::Time deltaTime) = 0;

	// Has director finished
	bool isComplete();

	// Adds named object to be tracked
	void trackObject(std::string levelName, std::string objName);

	// Name object exists
	bool hasObject(std::string objName);

	// Try and get named object
	template <class T>
	T* getObject(std::string objName);

	// sets delay on director action
	void wait(sf::Time wait);

protected:

	virtual void initialize() {};

	// Any named object in level
	struct ScriptedObject {
		GameObject* obj = nullptr;
		std::string onLevel;
	};

	// Pointers to zone, objmanager and camera
	Zone *zone;
	ObjectManager *objMan;
	GameCamera *cam;

	// Director has started up
	bool initialized = false;
	// Director has run it's course and no longer needs to be run
	bool complete = false;

	// Delays director action
	sf::Time waitTimer;
	
	// Stores pointers to named objects
	std::map<std::string, ScriptedObject> objTracker;
	void updateTracker();
};

//#include "AlphaDirector.hpp"

#endif