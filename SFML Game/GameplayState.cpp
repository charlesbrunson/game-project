
#include <SFML/Graphics.hpp>

#include "GameplayState.hpp"
#include "GameCamera.hpp"
#include "Player.hpp"
#include "PlayerRef.hpp"

#include "LevelList.hpp"
#include "PixelSnap.hpp"

//other states
#include "UIPauseState.hpp"
#include "TransitionHorizontalState.hpp"
#include "TransitionVerticalState.hpp"

#include <cmath>
#include <fstream>

GameplayState::GameplayState(ResourceLoader *rloader) : State(rloader) {
	timeScale = 1.f;

	res = rloader;

	LevelDef::initLevelDefs();

	activeLevel = 0;
	lastCheckpoint = LevelDef::checkpoints[activeLevel].begin();

	//start loading zone
	start(*lastCheckpoint);
}

void GameplayState::start(sf::String levelstart, bool ignorePlayer) {
	
	playerDeathTimer = sf::Time::Zero;

	if (director != nullptr) {
		delete director;
		director = nullptr;
	}
	if (objMan != nullptr) {
		if (!ignorePlayer) {
			objMan->clear();
			delete objMan;
			objMan = nullptr;
		}
		else {
			objMan->clearExceptPlayer();
		}
	}
	if (currentZone != nullptr) {
		delete currentZone;
		currentZone = nullptr;
	}
	

	currentZone = new Zone(res);
	if (!ignorePlayer || objMan == nullptr) {
		objMan = new ObjectManager(res, nullptr);
	}

	currentZone->createZone(levelstart, objMan, &cam, ignorePlayer);
	objMan->gameLevel = currentZone->getCurrentLevel();

	updatePlayerData(sf::Time::Zero);

	//director = new AlphaDirector(currentZone, objMan, &cam);
}

//update gameplay state
void GameplayState::update(sf::Time deltaTime) {

	removeStateOnChange = true;

	sf::Time updateTime = deltaTime * timeScale;

	updateGame(updateTime);
}

void GameplayState::updateGame(sf::Time deltaTime) {
	
	if (Controls::isPressed(Controls::Input::START)) {
		//pause menu
		Controls::confirmPress(Controls::Input::START);
		removeStateOnChange = false;
		toNextState = true;
		nextState = new UIPauseState(this, res);

		return;
	}

	Player *plr = nullptr;
	if (objMan && currentZone) {
		plr = objMan->getPlayer();
		gPlayer = plr;
		if (plr) {
			gPlayerPos = plr->getPosition();

			if (plr->getCollision().intersects((sf::FloatRect)currentZone->getCurrentLevel()->levelArea)) {
				std::vector<Transition> *transitions = currentZone->getCurrentLevel()->getLevelTransitions();
				for (std::vector<Transition>::iterator trans = transitions->begin(); trans != transitions->end(); trans++) {

					if (plr->getCollision().intersects(trans->box)) {

						removeStateOnChange = false;
						toNextState = true;
						if (trans->dir == Cardinal::EAST || trans->dir == Cardinal::WEST) {
							nextState = new TransitionHorizontalState(this, &*trans, res);
						}
						else {
							nextState = new TransitionVerticalState(this, &*trans, res);
						}
						return;
					}
				}
			}
		}
	}

	//update objects
	if (pauseTime <= sf::Time::Zero)
		objMan->update(deltaTime, &score, !playerAlive);
	else {
		objMan->update(sf::Time::Zero, &score, !playerAlive);
		pauseTime -= deltaTime;
	}
	
	//update camera
	updateCamera(deltaTime);

	//update level sprite
	int drawMargin = 0;
	//sf::IntRect camArea = sf::IntRect(getCameraArea());
	currentZone->update(deltaTime);
	currentZone->getCurrentLevel()->update(playerAlive ? deltaTime : sf::Time::Zero, getCameraArea(), cam.getPosition());

	if (plr) {
		//hud.update(cam.getPosition(), plr->health, score, lives, 0, 0, deltaTime);

		score = std::min(score, scoreMax);
		lives = std::min(lives, livesMax);

		playerAlive = !plr->isDead();
	}
			
	/*
	if (transitionEnabled && transitionTimer >= sf::Time::Zero) {
		if (transitionTimer == sf::Time::Zero){
			if (!transitionIn)
				transitionEnabled = false;
		}
		else
			transitionTimer = std::max(transitionTimer - deltaTime, sf::Time::Zero);
	}
	*/

	if (alertBreak > sf::Time::Zero) {
		alertBreak -= deltaTime;
	}
	else {
		readAlerts();
	}


	if (director != nullptr)
		director->update(deltaTime);

	/*
	if (!playerAlive) {
		if (playerDeathTimer < playerDeathDuration) {
			playerDeathTimer += deltaTime;
		}
		else {
			lives--;
			if (lives > 0) {
				//restart at checkpoint
				start(*lastCheckpoint);
				score = scoreLastCheckpoint;
				nextState = new CheckpointState(this, lives, res);
				toNextState = true;
			}
			else {
				//restart level
				score = 0;
				scoreLastCheckpoint = 0;
				lives = livesStart;
				lastCheckpoint = LevelDef::checkpoints[activeLevel].begin();
				start(*lastCheckpoint);
				nextState = new GameOverState(false, this, res);
				toNextState = true;
			}
		}
	}
	*/
}

void GameplayState::readAlerts() {
	

	Player *plr = objMan->getPlayer();

	while (AlertSystem::hasAlerts()) {
		AlertSystem::Alert a = AlertSystem::popAlert();
		switch (a.alertType) {

			//stop reading alerts for this amount of time
		case AlertSystem::BREAK:
			alertBreak = sf::milliseconds(a.argNum);
			return;
			break;

		case AlertSystem::PLAYER_DEAD:
			playerAlive = false;
			plr->disableInput();
			plr->inputAutoEnableTimer = playerDeathDuration;
			//MusicPlayer::stopAll();
			break;

		case AlertSystem::GOT_ONEUP:
			if (lives == livesMax) {
				score = std::min(scoreMax, score + 10000);
			}
			else {
				score = std::min(scoreMax, score + 1000);
				lives++;
			}
			break;

		case AlertSystem::GOTO_LEVEL:

			start(a.argString, true);
			plr = objMan->getPlayer();
			plr->setLevel(currentZone->getCurrentLevel());
			plr->setPosition(a.argPos);
			break;


		case AlertSystem::GOTO_NEXT_CHECKPOINT:

			if ((lastCheckpoint + 1) != LevelDef::checkpoints[activeLevel].end()) {

				lastCheckpoint++;
				start(*lastCheckpoint, true);
				scoreLastCheckpoint = score;
				plr = objMan->getPlayer();
				updatePlayerData(sf::Time::Zero);

			}
			break;

			//pause gameplay for this amount of time
		case AlertSystem::PAUSE:

			if (director != nullptr) {
				objMan->update(sf::Time::Zero, &score, !playerAlive);
				director->update(sf::Time::Zero);
				pauseTime = std::max(pauseTime, sf::milliseconds(a.argNum));
				director->wait(pauseTime);
			}
			break;

		case AlertSystem::CAM_SHAKE:
			cam.shake(a.argPos, sf::milliseconds(a.argNum), 4);
			break;

			/*
		case AlertSystem::TRANSITION_ENTER:
			transitionEnabled = true;
			transitionIn = true;
			transitionDuration = sf::milliseconds(a.argNum);
			transitionTimer = transitionDuration;
			break;

		case AlertSystem::TRANSITION_EXIT:
			transitionEnabled = true;
			transitionIn = false;
			transitionDuration = sf::milliseconds(a.argNum);
			transitionTimer = transitionDuration;
			break;*/

			/*
		case AlertSystem::BEAT_THE_GAME:
			score = 0;
			scoreLastCheckpoint = 0;
			lives = livesStart;
			lastCheckpoint = LevelDef::checkpoints[activeLevel].begin();
			start(*lastCheckpoint);
			nextState = new GameOverState(true, this, res);
			toNextState = true;
			break;
		*/
		}
	}
}

void GameplayState::updatePlayerData(sf::Time deltaTime) {

	Player *plr = objMan->getPlayer();
	gPlayer = plr;
	if (plr != nullptr) {
		plr->setLevel(currentZone->getCurrentLevel());
		plr->setPosition(currentZone->getCurrentLevel()->playerStartPos);
		gPlayerPos = plr->getPosition();
		cam.setPosition(gPlayerPos, true);
		cam.setModeSmart(plr);
		updateCamera(deltaTime);
		//hud.update(cam.getPosition(), plr->health, score, lives, 0, 0, deltaTime);
	}
}

void GameplayState::updateCamera(sf::Time updateTime) {
	if (cam.getMode() != GameCamera::CamModes::SMART && objMan->getPlayer() != nullptr) {
		GameObject *e = objMan->getPlayer();
		cam.setModeSmart(e);
		cam.setPosition(e->getPosition());
	}

	cam.update(updateTime);

	cameraPos = cam.getPosition();

	sf::IntRect area = currentZone->getCurrentLevel()->levelArea;

	int camTop = area.top + (GAMEHEIGHT / 2);
	int camRight = area.left + area.width - (GAMEWIDTH / 2);
	int camBottom = area.top + area.height - (GAMEHEIGHT / 2);
	int camLeft = area.left + (GAMEWIDTH / 2);

	//keep camera from moving out of level
	//check vertical bounds
	if (area.height <= GAMEHEIGHT) {
		cameraPos.y = (camBottom + camTop) / 2;
	}
	else if (cameraPos.y < camTop) {
		cameraPos.y = camTop;
	}
	else if (cameraPos.y > camBottom) {
		cameraPos.y = camBottom;
	}
	//check horizontal bounds
	if (area.width <= GAMEWIDTH) {
		cameraPos.x = (camLeft + camRight) / 2;
	}
	else if (cameraPos.x < camLeft) {
		cameraPos.x = camLeft;
	}
	else if (cameraPos.x > camRight) {
		cameraPos.x = camRight;
	}

	cam.setPosition(cameraPos);

	cameraPos = cam.getPosition();
	gCameraPos = cam.getPosition();
}

void GameplayState::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	
	Level *lvl = currentZone->getCurrentLevel();
	sf::IntRect area = lvl->levelArea;

	//draw level background color, only drawing where level area and game camera intersects
	sf::FloatRect levelRect(area);
	sf::FloatRect intersection;
	levelRect.intersects(getCameraArea(), intersection);

	//snap to pixel
	intersection.left = snapToPixel(intersection.left);
	intersection.top = snapToPixel(intersection.top);
	intersection.width = snapToPixel(intersection.width);
	intersection.height = snapToPixel(intersection.height);

	sf::RectangleShape levelBG;
	levelBG.setPosition(sf::Vector2f(intersection.left, intersection.top));
	levelBG.setSize(sf::Vector2f(intersection.width, intersection.height));
	levelBG.setFillColor(lvl->getBGColor());
	target.draw(levelBG, states);

	//draw bg sprite
	if (lvl->getBGSprite().getTexture())
		target.draw(lvl->getBGSprite(), states);

	//draw parallax layers
	for (auto i = lvl->getParallaxLayers()->rbegin(); i != lvl->getParallaxLayers()->rend(); i++)
		target.draw(i->second, states);

	//draw under-background objects
	std::vector<std::unique_ptr<GameObject>>::const_iterator obj;
	for (obj = objMan->getObjects()->begin(); obj != objMan->getObjects()->end(); obj++) {
		if (!obj->get()->drawUnderBackground())
			break;
		else
			target.draw(*obj->get(), states);
	}

	//draw level background
	target.draw(*lvl->getBackgroundLayer(), states);
		
	//draw under-object effects
	for (std::vector<std::unique_ptr<Effect>>::const_iterator f = objMan->getEffects()->begin(); f != objMan->getEffects()->end(); f++) {
		if (f->get()->layer == Effect::EffectLayer::UNDER) 
			target.draw(*f->get(), states);
	}

	//draw over-background objects
	for (; obj != objMan->getObjects()->end(); obj++) {
		target.draw(*obj->get(), states);
	}

	//draw over-object effects
	for (std::vector<std::unique_ptr<Effect>>::const_iterator f = objMan->getEffects()->begin(); f != objMan->getEffects()->end(); f++) {
		if (f->get()->layer == Effect::EffectLayer::OVER) 
			target.draw(*f->get(), states);
	}

	//draw level foreground
	target.draw(*lvl->getForegroundLayer(), states);

	//draw level decorative layer
	target.draw(*lvl->getDecorativeLayer(), states);

	//draw black borders if level is too small
	if (area.width < GAMEWIDTH) {
		float hMargin = std::max(((float)GAMEWIDTH - area.width) / 2, 0.f);
		float vMargin = std::max(((float)GAMEHEIGHT - area.height) / 2, 0.f);

		sf::RectangleShape black;
		black.setFillColor(sf::Color::Black);

		black.setPosition(-hMargin, -vMargin);
		black.setSize(sf::Vector2f(hMargin, GAMEHEIGHT));
		target.draw(black);

		black.setPosition(GAMEWIDTH - (hMargin * 2), -vMargin);
		target.draw(black);

	}
	if (area.height < GAMEHEIGHT) {
		float hMargin = std::max(((float)GAMEWIDTH - area.width) / 2, 0.f);
		float vMargin = std::max(((float)GAMEHEIGHT - area.height) / 2, 0.f);

		sf::RectangleShape black;
		black.setFillColor(sf::Color::Black);
		if (area.width >= GAMEWIDTH) {

			black.setPosition(-hMargin, -vMargin);
			black.setSize(sf::Vector2f(GAMEWIDTH, vMargin));
			target.draw(black);

			black.setPosition(-hMargin, GAMEHEIGHT - (vMargin * 2));
			target.draw(black);
		}
		else {

			black.setPosition(0, -vMargin);
			black.setSize(sf::Vector2f((float)area.width, vMargin));
			target.draw(black);

			black.setPosition(0, GAMEHEIGHT - (vMargin * 2));
			target.draw(black);
		}
	}
#ifdef _DEBUG
	if (Gameplay_Globals::Debug::levelCollision) {

		sf::RectangleShape r;

		//top - red
		r.setFillColor(sf::Color(255, 255, 255, 128));
		for (std::vector<sf::FloatRect>::const_iterator it = lvl->getUpCol()->begin(); it != lvl->getUpCol()->end(); it++) {
			r.setPosition(sf::Vector2f(it->left, it->top));
			r.setSize(sf::Vector2f(it->width, it->height - 8));
			target.draw(r);
		}

		//right - green
		r.setFillColor(sf::Color(0, 255, 0, 128));
		for (std::vector<sf::FloatRect>::const_iterator it = lvl->getRightCol()->begin(); it != lvl->getRightCol()->end(); it++) {
			r.setPosition(sf::Vector2f(it->left + 8, it->top));
			r.setSize(sf::Vector2f(it->width - 8, it->height));
			target.draw(r);
		}

		//left - blue
		r.setFillColor(sf::Color(0, 0, 255, 128));
		for (std::vector<sf::FloatRect>::const_iterator it = lvl->getLeftCol()->begin(); it != lvl->getLeftCol()->end(); it++) {
			r.setPosition(sf::Vector2f(it->left, it->top));
			r.setSize(sf::Vector2f(it->width - 8, it->height));
			target.draw(r);
		}

		//down - yellow
		r.setFillColor(sf::Color(255, 0, 0, 128));
		for (std::vector<sf::FloatRect>::const_iterator it = lvl->getDownCol()->begin(); it != lvl->getDownCol()->end(); it++) {
			r.setPosition(sf::Vector2f(it->left, it->top + 8));
			r.setSize(sf::Vector2f(it->width, it->height - 8));
			target.draw(r);
		}
	}
#endif

	//apply shader if transition is active
	/*
	if (sf::Shader::isAvailable() && transitionEnabled && transitionTimer >= sf::Time::Zero) {
		sf::RectangleShape transition;
		transition.setFillColor(sf::Color::Black);
		transition.setPosition(cam.getPosition().x - GAMEWIDTH / 2, cam.getPosition().y - GAMEHEIGHT / 2);
		transition.setSize(sf::Vector2f(GAMEWIDTH, GAMEHEIGHT));


		sf::Vector2f topLeft = cam.getPosition() - sf::Vector2f(GAMEWIDTH / 2, GAMEHEIGHT / 2);
		float percent = transitionTimer.asSeconds() / transitionDuration.asSeconds();

		target.draw(transition, res->getTransitionShader(std::min(percent, 1.f), transitionIn, topLeft));
	}
	*/

	//draw hud
	//target.draw(hud, states);
};