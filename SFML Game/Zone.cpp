
//#include <fstream>
//#include <sstream>

//#include "rapidxml.hpp"
//#include "rapidxml_iterators.hpp"
//#include "rapidxml_utils.hpp"

#include <assert.h>

#include "Globals.hpp"
#include "Zone.hpp"
#include "Player.hpp"
#include "LevelLoader.hpp"


//------------------------------------------------------------------
//constructs zone and starting level
//------------------------------------------------------------------

void Zone::createZone(sf::String startLevel, ObjectManager *objMan, GameCamera *cam, bool ignorePlayer) {
	//construct level
	LevelArea area;
	LevelLoader::loadLevel(startLevel, area, getResources(), this);
	Log::msg("Loaded " + startLevel + "\n");
	objMan->gameLevel = area.level;

	activeLevels.insert(std::pair<sf::String, LevelArea>(startLevel, area));

	//data has been copied into map, reinitialize currentLevel
	//delete currentLevel;
	currentLevel = &activeLevels[startLevel];

	startingLevel = startLevel;

	//start loading adjacent levels
	beginLoadingAdjacentLevels();

	if (!ignorePlayer)
		objMan->clear();
	else
		objMan->clearExceptPlayer();


	objMan->readObjNodes(currentLevel->objects, currentLevel->level, ignorePlayer);

};

//------------------------------------------------------------------
//initiates level transitions
//------------------------------------------------------------------

void Zone::switchLevels(Transition destination, bool movePlayer, ObjectManager *objMan, GameCamera *cam) {
	//depreciated
	/*
	Log("Transitioning to " + destination.levelName + "\n");


	completeThread();

	sf::Vector2i currentLevelSize(currentLevel->level->levelArea.width, currentLevel->level->levelArea.height);
	LevelArea *nextLevel = &(activeLevels[destination.levelName]);

	sf::Vector2i nextLevelSize(nextLevel->level->levelArea.width, nextLevel->level->levelArea.height);
	sf::Vector2f offset(destination.leftside ? nextLevelSize.x : -currentLevelSize.x, destination.offset);

	// revert old objects back into nodes and update level node list
	saveObjNodeStates(objMan);
	
	// remove tile sprites of old level
	currentLevel->level->clearSpriteTiles();
	currentLevel->level->update(sf::Time::Zero, sf::FloatRect(), cam->getPosition());
	
	// swap current level with next level
	currentLevel = nextLevel;
	objMan->gameLevel = nextLevel->level;

	//move global objects and reset objects
	objMan->clearExceptGlobals();
	for (auto i = objMan->getObjects()->begin(); i != objMan->getObjects()->end(); i++) {
		//move object to next level
		GameObject *o = i->get();
		o->setPosition(o->getPosition() + offset);

		for (std::vector<Effect*>::iterator i = o->createdEffects.begin(); i != o->createdEffects.end(); i++) {
			(*i)->setPosition((*i)->getPosition() + offset);
		}
		for (std::vector<GameObject*>::iterator j = o->createdObjs.begin(); j != o->createdObjs.end(); j++) {
			(*j)->setPosition((*j)->getPosition() + offset);
		}

		if (o->objectType == GameObject::type::PLAYER) {
			//((Player*)o)->activeSubweapons.clear();
		}

		o->setLevel(nextLevel->level);
	}
	// update camera
	cam->setTargetPos(cam->getTargetPos() + offset);
	cam->setPosition(cam->getTargetPos());

	// read in object of current level
	objMan->readObjNodes(nextLevel->objects, objMan->gameLevel, true);

	//purge disconnected level
	purgeInactiveLevels();

	//start loading adjacent levels
	beginLoadingAdjacentLevels();
	*/
};
void Zone::purgeInactiveLevels() {
	for (auto j = activeLevels.begin(); j != activeLevels.end();) {
		if (!currentLevel->level->hasTransition(j->first) && currentLevel->levelName != j->first) {
			Log::msg("Deleting " + j->first + "\n");
			delete j->second.level;
			j = activeLevels.erase(j);
		}
		else {
			j++;
		}
	}
}

void Zone::saveObjNodeStates(ObjectManager *objMan) {
	std::vector<objNode> *newNodes = objMan->getObjectNodes();
	currentLevel->objects.clear();
	currentLevel->objects.insert(currentLevel->objects.end(), newNodes->begin(), newNodes->end());
}

//active counter
int Zone::getTimesActive(sf::String levelname) {
	auto f = numTimesActive.find(levelname);
	if (f != numTimesActive.end()) {
		return f->second;
	}
	else {
		return -1;
	}
};

void Zone::incrementActiveCount(sf::String levelname) {

	auto f = numTimesActive.find(levelname);
	assert(f != numTimesActive.end());
	f->second++;
}

void Zone::addActiveCounter(sf::String levelname) {
	numTimesActive.insert(std::pair<sf::String, int>(levelname, 0));
}

//background level loading thread
void Zone::beginLoadingAdjacentLevels() {

	//ensure current thread is completed
	completeThread();

	//start new thread
	if (!currentLevel->level->getLevelTransitions()->empty()) {

		//copy some needed data to separate from main thread
		std::vector<sf::String> *activeNames = new std::vector<sf::String>;
		for (const auto& p : activeLevels)
			activeNames->push_back(p.first);

		std::vector<Transition> *list = currentLevel->level->getLevelTransitions();
		std::vector<Transition> *trans = new std::vector<Transition>(*list);

		//heap objects get deleted in loadAdjacentLevels once it's done with them
		loaderThreadActive = true;
		adjacentLevelLoader = std::thread(loadAdjacentLevels, activeNames, trans, this);
		adjacentLevelLoader.detach();
	}
};

void Zone::completeThread() {
	while (loaderThreadActive) {
		sf::sleep(sf::microseconds(1));
	}
	if (!levelsToAdd.empty()) {
		activeLevels.insert(levelsToAdd.begin(), levelsToAdd.end());
		levelsToAdd.clear();
	}
}

//thread function that loads levels in advance for level transitions
void Zone::loadAdjacentLevels(std::vector<sf::String> *activeNames, std::vector<Transition> *trans, Zone *zone) {

	for (std::vector<Transition>::const_iterator t = trans->begin(); t != trans->end(); t++) {
		bool toInsert = true;
		for (std::vector<sf::String>::const_iterator name = activeNames->begin(); name != activeNames->end(); name++) {
			if (t->levelName == *name) {
				toInsert = false;
				break;
			}
		}
		if (toInsert) {
			LevelArea area;
			LevelLoader::loadLevel(t->levelName, area, zone->getResources(), zone);

			zone->levelsToAdd.insert(std::pair<sf::String, LevelArea>(t->levelName, area));
			Log::msg("Loaded " + t->levelName + "\n");
		}
	}

	delete activeNames;
	delete trans;

	zone->loaderThreadActive = false;
};