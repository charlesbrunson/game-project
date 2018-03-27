#include "trans/TransitionState.hpp"

#include <algorithm>

#include "obj/plr/PlayerState.hpp"


const sf::Time TransitionState::transDuration = sf::seconds(1.f);

TransitionState::TransitionState(GameplayState *game, Transition *activatedTransition) {
	//StateName = STATE_TRANSITION;

	gameplayState = game;
	objMan = game->getObjectMan();
	transition = *activatedTransition;
	zone = gameplayState->getZone();

	zone->completeThread();
	toLevelArea = zone->getLevelByName(transition.levelName);
	assert(toLevelArea != nullptr);
	
	fract = 0.f;

	plr = objMan->getPlayer();
	//figure player start
	if (plr != nullptr) {		
		plrStart = plr->getPosition();

		//find any parented effects attached to player
		auto effects = objMan->getEffects();
		for (auto e = effects->begin(); e != effects->end(); e++) {
			Entity* parent = e->get()->getParent();
			if (parent != nullptr && parent == (Entity*)plr) {
				parentedEffects.push_back(e->get());
			}
		}
	}

	camStart = gameplayState->cameraPos;
	cameraPos = camStart;

	isFake = (toLevelArea->level == zone->getCurrentLevel());
	sf::Time t = zone->getZoneLifetime();
	if (isFake) {
		fakeLevel = new FakeLevel(toLevelArea->level);
		for (auto i = fakeLevel->parallaxLayers.begin(); i != fakeLevel->parallaxLayers.end(); i++)
			i->second.syncTimersWithZone(t);
		fakeLevel->backLayer.syncTimersWithZone(t);
		fakeLevel->foreLayer.syncTimersWithZone(t);
		fakeLevel->decorLayer.syncTimersWithZone(t);
	}
	else {
		toLevelArea->level->syncLayerTimers(t);
	}
}

TransitionState::~TransitionState() {
	if (isFake)
		delete fakeLevel;
}

void TransitionState::initNextObjects() {

	// save objnodes from current level
	zone->saveObjNodeStates(objMan);

	// move globals to point to new level
	auto objects = objMan->getObjects();
	for (auto o = objects->begin(); o != objects->end(); o++) {
		GameObject *obj = o->get();
		if (obj->isGlobalObject()) {
			obj->setLevel(toLevelArea->level);

			for (auto j = obj->createdObjs.begin(); j != obj->createdObjs.end(); j++) {
				(*j)->setLevel(toLevelArea->level);
			}
		}
		else {
			//mark rest to be deleted later
			obj->setLevel(nullptr);
			for (auto j = obj->createdObjs.begin(); j != obj->createdObjs.end(); j++) {
				(*j)->setLevel(nullptr);
			}
		}
	}

	// add objects from next level to objman
	//offset them to appear in the level
	objMan->readObjNodes(toLevelArea->objects, toLevelArea->level, true, -toLvlOffset);
	
}

void TransitionState::update(sf::Time deltaTime) {

	transTimer += deltaTime;
	fract = transTimer / transDuration;

	//mid transition
	if (fract < 1.f) {

		//move player
		if (plr != nullptr) {
			float left = std::min(plrStart.x, plrEnd.x);
			float right = std::max(plrStart.x, plrEnd.x);

			float up = std::min(plrStart.y, plrEnd.y);
			float down = std::max(plrStart.y, plrEnd.y);

			float xPos = left + (right - left) * (plrStart.x < plrEnd.x ? fract : 1 - fract);
			float yPos = up + (down - up) * (plrStart.y < plrEnd.y ? fract : 1 - fract);

			plr->setPosition(sf::Vector2f(xPos, yPos));

			//move any effects parented to player as well
			for (auto effect = parentedEffects.begin(); effect != parentedEffects.end(); effect++) {
				(*effect)->update(sf::Time::Zero);
			}
		}
		//move camera
		{
			cameraPos = camStart + (camEnd - camStart) * fract;
		}
		gCameraPos = cameraPos;

		// update current level
		gameplayState->getZone()->getCurrentLevel()->update(
			sf::Time::Zero,
			getCameraArea(),
			gCameraPos);

		sf::FloatRect offsetCam(getCameraArea());
		offsetCam.left += toLvlOffset.x;
		offsetCam.top += toLvlOffset.y;

		// update fakelevel
		if (isFake) {
			updateFakeLevel(
				sf::Time::Zero,
				offsetCam,
				gCameraPos + toLvlOffset);
		}
		else {
			toLevelArea->level->update(
				sf::Time::Zero,
				offsetCam,
				gCameraPos + toLvlOffset);
		}

	}
	//complete transition
	else {

		Zone *z = gameplayState->getZone();
		ObjectManager *objMan = gameplayState->getObjectMan();

		// swap current level and next level
		z->setCurrentLevelArea(toLevelArea);
		objMan->gameLevel = toLevelArea->level;

		// delete old objects
		objMan->clearObjectsNotInLevel();

		// move new objects + camera back to actually be in level
		for (auto o = objMan->getObjects()->begin(); o != objMan->getObjects()->end(); o++) {
			o->get()->setPosition(o->get()->getPosition() + toLvlOffset);

			std::vector<Effect*>* effects = &o->get()->createdEffects;
			for (auto e = effects->begin(); e != effects->end(); e++) {
				(*e)->setPosition((*e)->getPosition() + toLvlOffset);
			}

			std::vector<GameObject*>* objects = &o->get()->createdObjs;
			for (auto c = objects->begin(); c != objects->end(); c++) {
				(*c)->setPosition((*c)->getPosition() + toLvlOffset);
			}
		}

		// move effects
		for (auto o = objMan->getEffects()->begin(); o != objMan->getEffects()->end(); o++) {
			o->get()->setPosition(o->get()->getPosition() + toLvlOffset);
		}

		// remove levels not directly connected to this one
		z->purgeInactiveLevels();

		// begin loading adjacent levels
		z->beginLoadingAdjacentLevels();

		//delete non parented effects
//		if (!isFake) {
//			for (auto e = objMan->getEffects()->begin(); e != objMan->getEffects()->end(); e++) {
//				if (e->get()->getParent() != plr) {
//					e->get()->toDelete = true;
//				}
//			}
//		}

		// update camera
		GameCamera *cam = gameplayState->getGameCamera();
		cam->setPosition(cam->getPosition() + toLvlOffset);
		cam->setTargetPos(cam->getTargetPos() + toLvlOffset);
		gameplayState->updateCamera(sf::Time::Zero);
		gCameraPos = cam->getPosition();
		
		// update current level
		Level* lvl = z->getCurrentLevel();
		lvl->syncLayerTimers(z->getZoneLifetime());
		lvl->update(sf::Time::Zero, getCameraArea(), gCameraPos);

		if (transition.dir == Cardinal::NORTH && plr) {
			//force player up on north transition
			plr->setVelY(std::min(PlayerState::jumpVel, plr->vel().y / 2.f));
		}

		// relinquish control back to gameplaystate
		removeStateOnChange = true;
		toNextState = true;
		nextState = gameplayState;
	}
	updateBGSprites();
}


void TransitionState::updateFakeLevel(sf::Time deltaTime, sf::FloatRect displayArea, sf::Vector2f camCenter) {

	sf::FloatRect intersection;
	displayArea.intersects(sf::FloatRect(fakeLevel->levelArea), intersection);
	
	sf::IntRect gridArea;
	gridArea.left = static_cast<int>(floorf(intersection.left / (float)tileSpacing));
	gridArea.top = static_cast<int>(floorf(intersection.top / (float)tileSpacing));
	gridArea.width = static_cast<int>(ceilf((intersection.left + intersection.width) / (float)tileSpacing)) - gridArea.left;
	gridArea.height = static_cast<int>(ceilf((intersection.top + intersection.height) / (float)tileSpacing)) - gridArea.top;

	fakeLevel->decorLayer.update(deltaTime, gridArea, intersection, camCenter);
	fakeLevel->foreLayer.update(deltaTime, gridArea, intersection, camCenter);
	fakeLevel->backLayer.update(deltaTime, gridArea, intersection, camCenter);

	for (auto i = fakeLevel->parallaxLayers.begin(); i != fakeLevel->parallaxLayers.end(); i++)
		i->second.update(deltaTime, gridArea, intersection, camCenter);
}

void TransitionState::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	Level* lvl = gameplayState->getZone()->getCurrentLevel();
	Level* nextLevel = toLevelArea->level;
	auto objs = gameplayState->getObjectMan()->getObjects();
	auto effects = gameplayState->getObjectMan()->getEffects();

	sf::Shader* offsetter = RL()->getShader("shaders/offset.vert");
	if (offsetter) 
		offsetter->setUniform("offset", -toLvlOffset);
	
	//background
	drawBG(target, states);
	{
		for (auto i = lvl->getParallaxLayers()->rbegin(); i != lvl->getParallaxLayers()->rend(); i++)
			target.draw(i->second, states);

		if (isFake) {
			for (auto j = fakeLevel->parallaxLayers.rbegin(); j != fakeLevel->parallaxLayers.rend(); j++)
				target.draw(j->second, offsetter);
		}
		else {
			for (auto j = nextLevel->getParallaxLayers()->rbegin(); j != nextLevel->getParallaxLayers()->rend(); j++)
				target.draw(j->second, offsetter);
		}
	}

	//draw under-background objects
	std::vector<std::unique_ptr<GameObject>>::const_iterator obj;
	for (obj = objs->begin(); obj != objs->end(); obj++) {
		if (!obj->get()->drawUnderBackground())
			break;
		else
			target.draw(*obj->get(), states);
	}

	//draw level background
	target.draw(*lvl->getBackgroundLayer(), states);
	if (isFake)
		target.draw(fakeLevel->backLayer, offsetter);
	else
		target.draw(*nextLevel->getBackgroundLayer(), offsetter);

	//draw under-object effects
	for (std::vector<std::unique_ptr<Effect>>::const_iterator f = effects->begin(); f != effects->end(); f++) {
		if (f->get()->layer == Effect::EffectLayer::UNDER)
			target.draw(*f->get(), states);
	}

	//draw over-background objects
	for (; obj != objs->end(); obj++) {
		target.draw(*obj->get(), states);
	}

	//draw over-object effects
	for (std::vector<std::unique_ptr<Effect>>::const_iterator f = effects->begin(); f != effects->end(); f++) {
		if (f->get()->layer == Effect::EffectLayer::OVER)
			target.draw(*f->get(), states);
	}

	//draw level foreground
	target.draw(*lvl->getForegroundLayer(), states);
	if (isFake)
		target.draw(fakeLevel->foreLayer, offsetter);
	else
		target.draw(*nextLevel->getForegroundLayer(), offsetter);

	//draw level decorative layer
	target.draw(*lvl->getDecorativeLayer(), states);
	if (isFake)
		target.draw(fakeLevel->decorLayer, offsetter);
	else
		target.draw(*nextLevel->getDecorativeLayer(), offsetter);

	for (auto i = covers.begin(); i != covers.end(); i++) {
		target.draw(*i, states);
	}
}
