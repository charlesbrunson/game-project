
//#include <fstream>
//#include <sstream>

//#include "rapidxml.hpp"
//#include "rapidxml_iterators.hpp"
//#include "rapidxml_utils.hpp"

#include <assert.h>

#include "Globals.hpp"
#include "game/Zone.hpp"
#include "obj/plr/Player.hpp"
#include "game/lvl/LevelLoader.hpp"

//------------------------------------------------------------------
//constructs zone and starting level
//------------------------------------------------------------------

void Zone::createZone(std::string startLevel, ObjectManager *objMan, GameCamera*, bool ignorePlayer) {
	//construct level
	LevelArea area;
	LevelLoader::loadLevel(startLevel, area, this);
	Log::msg("Loaded " + startLevel);
	objMan->gameLevel = area.level;

	activeLevels.insert(std::pair<std::string, LevelArea>(startLevel, area));

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

//void Zone::switchLevels(Transition, bool, ObjectManager*, GameCamera*) {
	//depreciated
	/*
	Log("Transitioning to " + destination.levelName);


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
//};

void Zone::purgeInactiveLevels() {
	for (auto j = activeLevels.begin(); j != activeLevels.end();) {
		if (!currentLevel->level->hasTransition(j->first) && currentLevel->levelName != j->first) {
			Log::msg("Deleting " + j->first);
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
int Zone::getTimesActive(std::string levelname) {
	auto f = numTimesActive.find(levelname);
	if (f != numTimesActive.end()) {
		return f->second;
	}
	else {
		return -1;
	}
};

void Zone::incrementActiveCount(std::string levelname) {

	auto f = numTimesActive.find(levelname);
	assert(f != numTimesActive.end());
	f->second++;
}

void Zone::addActiveCounter(std::string levelname) {
	numTimesActive.insert(std::pair<std::string, int>(levelname, 0));
}

//background level loading thread
void Zone::beginLoadingAdjacentLevels() {

	//ensure current thread is completed
	completeThread();

	//start new thread
	if (!currentLevel->level->getLevelTransitions()->empty()) {

		//copy some needed data to separate from main thread
		std::vector<std::string> *activeNames = new std::vector<std::string>;
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
void Zone::loadAdjacentLevels(std::vector<std::string> *activeNames, std::vector<Transition> *trans, Zone *zone) {

	for (std::vector<Transition>::const_iterator t = trans->begin(); t != trans->end(); t++) {

		bool toLoad = std::find_if(activeNames->begin(), activeNames->end(), [&t](std::string n) {
			return n == t->levelName;
		}) == activeNames->end();

		if (toLoad) {
			LevelArea area;
			LevelLoader::loadLevel(t->levelName, area, zone);
			zone->levelsToAdd.insert(std::pair<std::string, LevelArea>(t->levelName, area));
			activeNames->push_back(t->levelName);
			Log::msg("Loaded " + t->levelName);
		}
	}

	delete activeNames;
	delete trans;

	zone->loaderThreadActive = false;
};
