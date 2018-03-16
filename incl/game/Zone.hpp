#ifndef ZONE_H
#define ZONE_H

#include <thread>

#include <SFML/System.hpp>
#include <stdexcept>
#include <assert.h>

#include "game/lvl/Node.hpp"
#include "game/lvl/Level.hpp"
#include "game/lvl/LevelSerializer.hpp"
#include "game/ObjectManager.hpp"
#include "game/GameCamera.hpp"
#include "res/ResourceLoader.hpp"

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
				Log::msg("Deleting " + it->first);

				if (it->second.level != nullptr) {
					delete it->second.level;
					it->second.level = nullptr;
				}

				it->second.objects.clear();
			}

			activeLevels.clear();
		}
	};

	void update(sf::Time deltaTime) { zoneLifetime += deltaTime; };

	void createZone(std::string startLevel, ObjectManager *objMan, GameCamera *cam, bool ignorePlayer = false);

	//transition to another level
	//void switchLevels(Transition destination, bool movePlayer, ObjectManager *objMan, GameCamera *cam);

	struct LevelArea {
		std::string levelName;
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
	std::string getCurrentLevelName() { return currentLevel->levelName; }
	
	int getTimesActive(std::string levelname);
	void incrementActiveCount(std::string levelname);
	void addActiveCounter(std::string levelname);

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

	std::string startingLevel;

	LevelArea* currentLevel;

	std::map<std::string, LevelArea> activeLevels;
	std::map<std::string, int> numTimesActive;

	//used by thread
	std::map<std::string, LevelArea> levelsToAdd;

	bool loaderThreadActive = false;
	std::thread adjacentLevelLoader;
	
	//thread function that loads levels in advance for level transitions
	static void loadAdjacentLevels(std::vector<std::string> *activeNames, std::vector<Transition> *trans, Zone *zone);

};

#endif
