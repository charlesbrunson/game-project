#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include <SFML/Graphics.hpp>

#include "State.hpp"
#include "ObjectManager.hpp"
#include "Zone.hpp"
#include "GameCamera.hpp"
#include "CamRef.hpp"

#include "ResourceLoader.hpp"
#include "Director.hpp"
#include "AlertSystem.hpp"

#include <sstream>

//the actual gameplay takes place here
class GameplayState : public State {
public:
	
	//StateName = STATE_GAMEPLAY;

	GameplayState();

	~GameplayState() {
		objMan->clear();
		AlertSystem::clearAlerts();
		delete currentZone;
		delete objMan;
		delete director;
	};

	void update(sf::Time deltaTime);
	void updateCamera(sf::Time deltaTime);

	bool passEvent(sf::Event e) {
		return true;
	};

	void start(std::string levelstart, bool ignorePlayer = false);
	
	void readAlerts();

	ObjectManager* getObjectMan() { return objMan; };
	Zone* getZone() { return currentZone; };
	GameCamera* getGameCamera() { return &cam; };


protected:
	//obj manager
	ObjectManager *objMan = nullptr;

	//level manager
	Zone *currentZone = nullptr;

	//director
	Director *director = nullptr;

	//gameplay hud
	//HUD hud;

	bool playerAlive = true;

	int score = 0;
	int scoreLastCheckpoint = 0;
	const int scoreMax = 9999999;

	int livesStart = 3;
	int lives = livesStart;
	const int livesMax = 99;

	void updatePlayerData(sf::Time deltaTime);

private:

	std::string levelStart;
	std::string levelCheckpoint;

	int activeLevel = 0;
	std::vector<std::string>::const_iterator lastCheckpoint;

	GameCamera cam;
	
	void updateGame(sf::Time deltaTime);

	sf::Time pauseTime = sf::Time::Zero;

	//debug
	float timeScale;
	
	int moveOut(Player &p, sf::FloatRect r);
	
	sf::Time alertBreak = sf::Time::Zero;

	sf::Time playerDeathDuration = sf::seconds(3.f);
	sf::Time playerDeathTimer = sf::Time::Zero;
		
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

#endif