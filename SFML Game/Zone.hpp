#ifndef ZONE_H
#define ZONE_H

#include <thread>

#include <SFML/System.hpp>
#include <stdexcept>
#include <assert.h>

#include "Node.hpp"
#include "Level.hpp"
#include "LevelSerializer.hpp"
#include "ObjectManager.hpp"
#include "GameCamera.hpp"
#include "ResourceLoader.hpp"

//manages levels, loads adjoining levels via threads to make level transitions more instant
//holds the objNodes for each level

class Zone {
public:

	Zone() {
		currentLevel = nullptr;
	};

	~Zone() {
		completeThread();
				
		if (!activeLevels.empty()) {
			for (auto it = activeLevels.begin(); it != activeLevels.end(); it++) {
				Log::msg("Deleting " + it->first + "\n");

				if (it->second.level != nullptr) {
					delete it->second.level;
				}

				it->second.objects.clear();
				//it->second.platforms.clear();
			}

			activeLevels.clear();
		}
	};

	void update(sf::Time deltaTime) { zoneLifetime += deltaTime; };

	void createZone(sf::String startLevel, ObjectManager *objMan, GameCamera *cam, bool ignorePlayer = false);

	//transition to another level
	void switchLevels(Transition destination, bool movePlayer, ObjectManager *objMan, GameCamera *cam);

	struct LevelArea {
		sf::String levelName;
		Level* level = nullptr;
		std::vector<objNode> objects;
	};
		
	LevelArea* getLevelByName(std::string name) {

		auto i = activeLevels.find(name);
		if (i != activeLevels.end())
			return &i->second;

		return nullptr; 
	};
	Level* getCurrentLevel() { return currentLevel->level; };
	std::thread* getLoaderThread() { return &adjacentLevelLoader; };
	sf::String getCurrentLevelName() { return currentLevel->levelName; }
	
	int getTimesActive(sf::String levelname);
	void incrementActiveCount(sf::String levelname);
	void addActiveCounter(sf::String levelname);

	const sf::Time getZoneLifetime() { return zoneLifetime; };

	void completeThread();
	void purgeInactiveLevels();

	void saveObjNodeStates(ObjectManager *objMan);

	void setCurrentLevelArea(LevelArea *area) {
		currentLevel = area;
	}
	void beginLoadingAdjacentLevels();

private:
	
	sf::Time zoneLifetime = sf::Time::Zero;

	sf::String startingLevel;

	LevelArea* currentLevel;

	std::map<sf::String, LevelArea> activeLevels;
	std::map<sf::String, int> numTimesActive;

	//used by thread
	std::map<sf::String, LevelArea> levelsToAdd;

	bool loaderThreadActive = false;
	std::thread adjacentLevelLoader;
	
	//thread function that loads levels in advance for level transitions
	static void loadAdjacentLevels(std::vector<sf::String> *activeNames, std::vector<Transition> *trans, Zone *zone);

};

#endif