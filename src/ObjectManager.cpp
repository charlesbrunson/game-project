#include "game/ObjectManager.hpp"

#include "obj/ObjectTypes.hpp"
#include "game/lvl/NodeBuilder.hpp"
#include "obj/TemplateUser.hpp"
#include "util/Math.hpp"
#include "game/CamRef.hpp"

#include "obj/plr/PlayerRef.hpp"

#include "phys/SurfaceCollision.hpp"

void ObjectManager::clear() {

	objects.clear(); 
	collidables.clear();
	effects.clear();
	sounds.clear();
	plr = nullptr;
	gPlayer = plr;
	while (!addObjects.empty()) {
		delete addObjects.back();
		addObjects.pop_back();
	}
}

void ObjectManager::clearExceptPlayer() {
	
	bool foundPlayer = false;
	std::unique_ptr<GameObject> plrPtr;

	for (std::vector<std::unique_ptr<GameObject>>::iterator i = objects.begin(); i != objects.end(); i++) {
		if (i->get()->objectType == GameObject::PLAYER) {

			foundPlayer = true;

			//pick player out of list, so it doesn't get deleted
			plrPtr.swap(*i);
			break;

		}
	}

	clear();

	if (foundPlayer) {
		//place player back in list
		insertObject(plrPtr.release());

	}

	while (!addObjects.empty()) {
		delete addObjects.back();
		addObjects.pop_back();
	}
}

void ObjectManager::clearExceptGlobals() {
	effects.clear();
	sounds.clear();

	auto i = getObjects()->begin();
	while (i != getObjects()->end()) {
		if (!i->get()->isGlobalObject()) {
			//remove

			if (i->get()->isCollidable()) {
				collidables.erase(i->get());
			}

			i = getObjects()->erase(i);

		}
		else {
			//ignore
			i++;
		}
	}

	while (!addObjects.empty()) {
		delete addObjects.back();
		addObjects.pop_back();
	}
}

void ObjectManager::clearObjectsNotInLevel() {
	//effects.clear();
	//sounds.clear();

	auto i = getObjects()->begin();
	while (i != getObjects()->end()) {
		if (i->get()->getLevel() != gameLevel) {
			//remove

			if (i->get()->isCollidable()) {
				collidables.erase(i->get());
			}

			i = getObjects()->erase(i);

		}
		else {
			//ignore
			i++;
		}
	}

	while (!addObjects.empty()) {
		delete addObjects.back();
		addObjects.pop_back();
	}
}

Player* ObjectManager::getPlayer() const {
	return plr;

	/*
	for (std::vector<std::unique_ptr<GameObject>>::const_iterator e = objects.begin(); e != objects.end(); e++) {
		if (e->get()->objectType == GameObject::PLAYER) {
			return (Player*)(e->get());
		}
	}*/
	//return nullptr;
}

GameObject* ObjectManager::getObjectByName(std::string name) {
	for (std::vector<std::unique_ptr<GameObject>>::const_iterator e = objects.begin(); e != objects.end(); e++) {
		if (e->get()->name == name) {
			return e->get();
		}
	}
	return nullptr;
}

std::vector<std::unique_ptr<GameObject>>* ObjectManager::getObjects() {
	return &objects;
}
std::vector<std::unique_ptr<Effect>>* ObjectManager::getEffects() {
	return &effects;
}
/*
std::vector<std::unique_ptr<GameObject>>* ObjectManager::getTriggers() {
	return &triggers;
}
*/

void ObjectManager::update(sf::Time deltaTime, int *score, bool onlyPlayer) {
	if (!onlyPlayer) {
		for (std::map<std::string, sf::Sound>::iterator sound = sounds.begin(); sound != sounds.end();) {
			if (sound->second.getStatus() != sf::Sound::Playing) {
				sound = sounds.erase(sound);
			}
			else {
				sound++;
			}
		}

		updateObjects(deltaTime, score);
		updateEffects(deltaTime);
	}
	else {

		GameObject *plr = getPlayer();
		plr->update(deltaTime);

		if (plr->collisionActive) {
			doCollision(plr);
		}

		plr->updateAnimation(deltaTime);
	}
}

void ObjectManager::updateObjects(sf::Time deltaTime, int *score) {

	bool sortObjects = false;
	
	if (!toRemove.empty())
		toRemove.clear();
	
	typedef std::vector<std::unique_ptr<GameObject>>::iterator objIt;

	//update all objects
	for (objIt obj = objects.begin(); obj != objects.end(); obj++) {
		GameObject *objPtr = obj->get();
		
		//add created effects
		while (!objPtr->createdEffects.empty()) {
			effects.push_back(std::unique_ptr<Effect>(objPtr->createdEffects.back()));
			objPtr->createdEffects.pop_back();
		}

		//add newly created objects to internal objrequests vector
		if (!objPtr->createdObjs.empty()) {
			addObjects.insert(addObjects.end(), objPtr->createdObjs.begin(), objPtr->createdObjs.end());
			objPtr->createdObjs.clear();
		}
		
		if (objPtr->toDelete) {
			//select this to be removed
			*score = *score + objPtr->scoreToAdd;
			objPtr->scoreToAdd = 0;
			toRemove.push_back(obj);
		}
		else {

			//update
			objPtr->update(deltaTime);

			if (objPtr->changedDrawPriority) {
				sortObjects = true;
				objPtr->changedDrawPriority = false;
			}

			if (objPtr->scoreToAdd > 0) {
				*score = *score + objPtr->scoreToAdd;
				objPtr->scoreToAdd = 0;
			}

			if (objPtr->isCollidable()) {
				collidables.at(objPtr)->updateCollider(deltaTime);
			}

			//collision
			if (objPtr->collisionActive) {
				doCollision(objPtr);
			}

			//interact with other objects
			if (objPtr->objectType == GameObject::type::PLAYER || objPtr->objectType == GameObject::type::PROJECTILE_PLAYER) {
				for (objIt objInt = objects.begin(); objInt != objects.end(); objInt++) {

					if (objPtr != objInt->get()) {
						objPtr->interact(objInt->get());
					}

				}
			}

			//update sprites
			objPtr->updateAnimation(deltaTime);
		}

		while (!objPtr->createdSounds.empty()) {
			Sound s;
			s.sound = objPtr->createdSounds.back();
			objPtr->createdSounds.pop_back();

			soundsToPlay.push_back(s);
			
		}

	}

	for (std::vector<Sound>::const_iterator it = soundsToPlay.begin(); it != soundsToPlay.end();) {

		std::string target = soundsToPlay.back().sound;

		if (sounds.find(target) == sounds.end()) {
			//TODO
			//sounds.insert(std::pair<std::string, sf::Sound>(target, sf::Sound(RL()->getSoundBuffer(target))));
		}

		if (sounds.at(target).getStatus() != sf::Sound::Status::Playing || sounds.at(target).getPlayingOffset() >= Gameplay_Globals::minSoundTime) {
			sounds.at(target).setVolume(volumeGame);
			sounds.at(target).play();
			it = soundsToPlay.erase(it);
		}
		else {
			if (soundsToPlay.back().timeoutTimer < Gameplay_Globals::soundTimeout) {
				soundsToPlay.back().timeoutTimer += deltaTime;
				it++;
			}
			else {
				it = soundsToPlay.erase(it);
			}
		}
	}


	//remove selected objects
	while (!toRemove.empty())
	{
		if (toRemove.back()->get()->isCollidable()) {
			collidables.erase(toRemove.back()->get());
		}

		if ((GameObject*)plr == toRemove.back()->get()) {
			plr = nullptr;
		}

		objects.erase(toRemove.back());
		toRemove.pop_back();
	}
	
	//create all requested objects
	while (!addObjects.empty()) {
		insertObject(addObjects.back());
		addObjects.pop_back();
	}

	if (sortObjects) {
		std::sort(objects.begin(), objects.end(), 
			[](std::unique_ptr<GameObject> const& a, std::unique_ptr<GameObject> const& b) {
			return a->drawPriority > b->drawPriority;
		});
	}
}

void ObjectManager::updateEffects(sf::Time deltaTime) {
	//update all active effects
	for (std::vector<std::unique_ptr<Effect>>::const_iterator it = effects.begin(); it != effects.end();) {
		if (it->get()->toDelete) {

			it = effects.erase(it);

		}
		else {

			it->get()->update(deltaTime);

			it++;
		}
	}
}

void ObjectManager::insertObject(GameObject* obj) {
	//places object into array while maintaining array order

	std::unique_ptr<GameObject> uPtr(obj);

	//prevent more than one player being made
	assert(plr == nullptr || obj->objectType != GameObject::PLAYER);

	for (std::vector<std::unique_ptr<GameObject>>::iterator e = objects.begin(); e != objects.end(); e++) {

		if (e->get()->drawPriority <= obj->drawPriority) {
			auto it = objects.insert(e, std::move(uPtr));
			if (it->get()->isCollidable()) {
				((Collidable*)it->get())->setParent((GameObject*)it->get());
				collidables.insert(std::pair<GameObject*, Collidable*>(it->get(), (Collidable*)it->get()->getCollidable()));
			}
			return;
		}
	}

	//add to end
	objects.push_back(std::move(uPtr));
	if (plr == nullptr && objects.back().get()->objectType == GameObject::PLAYER) {
		plr = (Player*)objects.back().get();
	}

	if (objects.back().get()->isCollidable()) {
		((Collidable*)objects.back().get())->setParent((GameObject*)objects.back().get());
		collidables.insert(std::pair<GameObject*, Collidable*>(objects.back().get(), (Collidable*)objects.back().get()->getCollidable()));
	}
}


//////COLLISION WITH LEVEL//////////////

//COLFIX

void ObjectManager::doCollision(GameObject* obj) {


	obj->getCollision().curCollisions.clear();

	std::vector<SurfaceCollision> collisions;

	doLevelCollision(obj, &collisions);

	std::stable_sort(collisions.begin(), collisions.end(),
		[&obj](SurfaceCollision a, SurfaceCollision b) {
			return Math::magnitude(b.newPos - obj->getPosition()) >
                	Math::magnitude(a.newPos - obj->getPosition());
		}
	);

	bool first = true;
	for (SurfaceCollision& c : collisions) {
		if (!first) {
			c.eval();
		}
		else {
			first = false;
		}

		if (c.isValid()) {


			Vec2 newVel = obj->getVelocity();
			newVel = Math::projection(newVel, Line(Point(), c.newPos - obj->getPosition()).getLeftHandUnit());

			obj->setVelocity(newVel);
			//obj->setPosition(obj->getPosition() + c.getOffset(), false);
			Vec2 offset = c.newPos - plr->getPosition();
			obj->setPosition(c.newPos, false);
			obj->collisionUp |= offset.y < 0.f;
			obj->collisionDown |= offset.y > 0.f;
			obj->collisionRight |= offset.x < 0.f && offset.y == 0.f;
			obj->collisionLeft |= offset.x > 0.f && offset.y == 0.f;

			Solid::colFlags f;
			f.collisionUp = offset.y < 0.f;
			f.collisionDown = offset.y > 0.f;
			f.collisionRight = offset.x < 0.f && offset.y == 0.f;
			f.collisionLeft = offset.x > 0.f && offset.y == 0.f;

			obj->getCollision().curCollisions.push_back(std::make_pair(c.getSurface(), f));

			/*
			if (c.getOffset().y != 0.f) {
				obj->setVelocity(sf::Vector2f(obj->getVelocity().x, 0.f));
				obj->setPosition(obj->getPosition() + c.getOffset(), false);
				obj->collisionUp |= c.getOffset().y < 0.f;
				obj->collisionDown |= c.getOffset().y > 0.f;

				if (obj->collisionUp) {
					obj->setGrounded(true);
				}

			}
			if (c.getOffset().x != 0.f) {
				obj->setVelocity(sf::Vector2f(0.f, obj->getVelocity().y));
				obj->setPosition(obj->getPosition() + c.getOffset(), false);
				obj->collisionRight |= c.getOffset().x < 0.f;
				obj->collisionLeft |= c.getOffset().x > 0.f;
			}
			*/
	 	}
	}


};

void ObjectManager::doLevelCollision(GameObject *obj, std::vector<SurfaceCollision>* collisions) {

	sf::IntRect gridBounds;
	gridBounds.left = (int)std::floor(obj->getCollision().left / (float)tileSpacing);
	gridBounds.top = (int)std::floor(obj->getCollision().top / (float)tileSpacing);
	gridBounds.width = (int)std::ceil((obj->getCollision().left + obj->getCollision().width) / (float)tileSpacing) - gridBounds.left;
	gridBounds.height = (int)std::ceil((obj->getCollision().top + obj->getCollision().height) / (float)tileSpacing) - gridBounds.top;

	for (int x = gridBounds.left; x < gridBounds.left + gridBounds.width; x++) {
		for (int y = gridBounds.top; y < gridBounds.top + gridBounds.height; y++) {
			auto svec = gameLevel->getSurfaceMap()->getSurfacesInGrid(GridVec2(x,y));

			if (svec == nullptr) {

				continue;
			}
			else {

				for (Surface& ss : *svec) {
					//SurfaceCollision col(&ss, obj->getCollision(), obj->getPrevFrameCollision());
					SurfaceCollision col(&ss, obj, GridVec2(x, y));

					if (col.isValid()) {
						collisions->push_back(col);	
					}
				}
			}
		}
	}

};

/*

void ObjectManager::doLevelCollision(GameObject *obj, std::vector<Collision> *collisions) {
	const sf::FloatRect c = obj->getCollision();
	const sf::FloatRect oldC = obj->getPrevFrameCollision();
	
	//create bounding box for object's current frame collision box and previous frame collision box
	const sf::FloatRect bb(Math::boundingBox(c, oldC));
		
	std::vector<sf::FloatRect> *rightCols = gameLevel->getRightCol();
	std::vector<sf::FloatRect> *leftCols = gameLevel->getLeftCol();
	std::vector<sf::FloatRect> *upCols = gameLevel->getUpCol();
	std::vector<sf::FloatRect> *downCols = gameLevel->getDownCol();
	
	auto makeCol = [](sf::FloatRect r, float mag, int dir) -> Collision {
		Collision col;
		col.rect = r;
		col.magnitude = mag;
		col.horizontal = (dir % 2 == 1);
		if (col.magnitude == 0.f)
			col.dir = dir;
		return col;
	};

	//do right
	//if (vel.x < 0.f) {
	if (c.left < oldC.left) {
		for (auto i = rightCols->cbegin(); i != rightCols->cend(); i++) {
			if (i->intersects(bb) && bb.left <= i->left + i->width && oldC.left >= i->left + i->width) {
				collisions->push_back(makeCol(*i, i->left + i->width - c.left, 1));
			}
		}
		
	}
	//do left
	//else if (vel.x > 0.f) {
	else if (c.left + c.width > oldC.left + oldC.width) {	
		for (auto i = leftCols->cbegin(); i != leftCols->cend(); i++) {
			if (i->intersects(bb) && bb.left + bb.width >= i->left && oldC.left + oldC.width <= i->left) {
				collisions->push_back(makeCol(*i, i->left - c.left - c.width, 3));
			}
		}
	}
	//do up
	//if (vel.y > 0.f && !obj->isDroppingThroughFloor()) {
	if (c.top > oldC.top && !obj->isDroppingThroughFloor()) {
		for (auto i = upCols->cbegin(); i != upCols->cend(); i++) {
			if (i->intersects(bb) && bb.top + bb.height >= i->top && oldC.top + oldC.height <= i->top) {
				collisions->push_back(makeCol(*i, i->top - c.top - c.height, 0));
			}
		}
	}
	//do down
	//else if (vel.y < 0.f) {
	else if (c.top + c.height < oldC.top + oldC.height) {
		for (auto i = downCols->cbegin(); i != downCols->cend(); i++) {
			if (i->intersects(bb) && bb.top <= i->top + i->height && oldC.top >= i->top + i->height) {
				collisions->push_back(makeCol(*i, i->top + i->height - c.top, 0));
			}
		}
	}
}

void ObjectManager::doCollidableCollision(Collidable *collidable, GameObject *obj, std::vector<Collision> *collisions) {
	//adaptation of regular collision algorithm
	
	sf::FloatRect c = obj->getCollision();
	sf::FloatRect oldC = obj->getPrevFrameCollision();
	
	std::vector<Collidable::Collider> *upCol = collidable->getColliders(Collidable::UP);
	std::vector<Collidable::Collider> *rightCol = collidable->getColliders(Collidable::RIGHT);
	std::vector<Collidable::Collider> *downCol = collidable->getColliders(Collidable::DOWN);
	std::vector<Collidable::Collider> *leftCol = collidable->getColliders(Collidable::LEFT);
	
	auto makeCol = [collidable](sf::FloatRect r, float mag) -> Collision {
		Collision col;
		col.rect = r;
		col.magnitude = mag;
		col.parent = collidable;
		col.parentVel = collidable->getParent()->getVelocity();
		return col;
	};

	//do right
	//if (v.x < 0.f) {
	if (c.left < oldC.left) {
		for (auto i = rightCol->cbegin(); i != rightCol->cend(); i++) {
			if (i->box.intersects(c)) {
				collisions->push_back(makeCol(i->box, i->box.left + i->box.width - c.left));
			}
		}
	}
	//do left
	//else if (v.x > 0.f) {
	else if (c.left + c.width > oldC.left + oldC.width) {
		for (auto i = leftCol->begin(); i != leftCol->end(); i++) {
			if (i->box.intersects(c)) {
				collisions->push_back(makeCol(i->box, i->box.left - c.left - c.width));
			}
		}
	}
	//do up
	//if (v.y > 0.f) {
	if (c.top > oldC.top) {
		for (auto i = upCol->begin(); i != upCol->end(); i++) {
			if (i->box.intersects(c)) {
				collisions->push_back(makeCol(i->box, i->box.top - c.top - c.height));
			}
		}
	}
	//do down
	//else if (v.y < 0.f) {
	else if (c.top + c.height < oldC.top + oldC.height) {
		for (auto i = downCol->begin(); i != downCol->end(); i++) {
			if (i->box.intersects(c)) {
				collisions->push_back(makeCol(i->box, i->box.top + i->box.height - c.top));
			}
		}
	}
};

void ObjectManager::validateCollisions(std::vector<Collision> *collisions) {

	//collision validation (one collision can make another physically impossible)
	// ex: b___________ <- player moving across this surface
	//			|
	//		   a|	<- a is invalidated by surface b
	//			|

	bool aHori;
	bool aMag;
	sf::FloatRect a;

	bool bHori;
	bool bMag;
	sf::FloatRect b;

	//tests if a is invalidated by b
	for (unsigned int j = 0; j < collisions->size(); j++) {

		aHori = (*collisions)[j].horizontal;
		aMag = (*collisions)[j].magnitude < 0;
		a = (*collisions)[j].rect;

		if ((*collisions)[j].magnitude == 0.f)
			continue;

		for (unsigned int k = 0; k < collisions->size(); k++) {

			bHori = (*collisions)[k].horizontal;
			bMag = (*collisions)[k].magnitude < 0;

			if (j == k || !(*collisions)[j].valid || aHori == bHori || (*collisions)[k].magnitude == 0.f)
				continue;

			b = (*collisions)[k].rect;

			if (aHori && !bHori) {
				if (aMag) {
					//leftwards surface
					if (b.left < a.left && b.left + b.width >= a.left) {
						if (bMag && b.top <= a.top) {
							(*collisions)[j].valid = false;
						}
						else if (!bMag && b.top + b.height >= a.top + a.height)	{
							(*collisions)[j].valid = false;
						}

					}
				}
				else {
					//rightwards surface
					if (b.left + b.width > a.left + a.width && b.left <= a.left + a.width) {
						if (bMag && b.top <= a.top) {
							(*collisions)[j].valid = false;
						}
						else if (!bMag && b.top + b.height >= a.top + a.height)	{
							(*collisions)[j].valid = false;
						}
					}
				}
			}
			else if (!aHori && bHori) {
				if (aMag) {
					//upwards surface
					if (b.top < a.top && b.top + b.height >= a.top) {

						if (bMag && b.left <= a.left) {
							(*collisions)[j].valid = false;
						}
						else if (!bMag && b.left + b.width >= a.left + a.width)	{
							(*collisions)[j].valid = false;
						}
					}
				}
				else {
					//downwards surface
					if (b.top + b.height > a.top + a.height && b.top <= a.top + a.height) {
						if (bMag && b.left <= a.left) {
							(*collisions)[j].valid = false;
						}
						else if (!bMag && b.left + b.width >= a.left + a.width)	{
							(*collisions)[j].valid = false;
						}
					}
				}
			}
		}
	}
}

void ObjectManager::resolveCollisions(GameObject *obj, std::vector<Collision> *collisions) {
	
	std::stable_sort(collisions->begin(), collisions->end(),
		[](Collision a, Collision b) {
			return abs(b.magnitude) > abs(a.magnitude);
		}
	);

	//resolve in order, double check they still apply
	//const Collision* col;
	int side;
	sf::FloatRect bb;

	for (const Collision& col : *collisions) {
		//col = &(*collisions)[i];

		if (!col.valid)
			continue;
		
		bb = Math::boundingBox(obj->getPrevFrameCollision(), obj->getCollision());
		if (bb.intersects(col.rect)) {

			side = -1;
			if (col.horizontal) {
				obj->setVelocity(sf::Vector2f(0, obj->getVelocity().y));
				obj->setPosition(sf::Vector2f(obj->getPosition().x + col.magnitude, obj->getPosition().y), false);
				
				if (col.magnitude < 0 || col.dir == 3) {
					obj->collisionLeft = true;
					side = Collidable::Direction::LEFT;
				}
				else if (col.magnitude > 0 || col.dir == 1) {
					obj->collisionRight = true;
					side = Collidable::Direction::RIGHT;
				}
			}
			else {
				obj->setVelocity(sf::Vector2f(obj->getVelocity().x, 0.f));
				obj->setPosition(sf::Vector2f(obj->getPosition().x, obj->getPosition().y + col.magnitude), false);
				
				if (col.magnitude < 0 || col.dir == 0) {
					obj->collisionUp = true;
					side = Collidable::Direction::UP;
					obj->setGrounded(true);
				}
				else if (col.magnitude > 0 || col.dir == 2) {
					obj->collisionDown = true;
					side = Collidable::Direction::DOWN;
				}
			}
			if (col.parent != nullptr && !obj->isCollidable()) {
				col.parent->addCollidingObj(obj, side);
			}
		}
	}
}
*/


void ObjectManager::readObjNodes(std::vector<objNode> &objList, Level* lvl, bool skipPlayer, sf::Vector2f offsetPos) {
	
	//try and find player first
	if (!skipPlayer) {
		for (const objNode& objN : objList) {
			if (objN.type == "PLAYER"){

				GameObject *plr = gBuildObject(objN, lvl);
				plr->setPosition(plr->getPosition() + offsetPos);

				if (plr == nullptr){
					Log::msg("WARNING: FAILED TO INSTANTIATE " + (std::string)objN.type + "\n");
				}
				else {
					gCameraPos = plr->getPosition();
					insertObject(plr);
				}
			}
		}
	}
	for (const objNode& objN : objList) {
		if (objN.type != "PLAYER" && !objN.isTemplate) {

			GameObject *obj = gBuildObject(objN, lvl);
			obj->setPosition(obj->getPosition() + offsetPos);

			if (obj->isTemplateUser()) {
				TemplateUser *user = (TemplateUser*)(obj->getTemplateUser());

				for (const objNode &objTemplate : objList) {
					if (objTemplate.isTemplate && objTemplate.name != "" && objTemplate.name == user->getTemplateName()) {
						user->setTemplateNode(objTemplate);
						break;
					}
				}
			}

			if (obj == nullptr) {
				Log::msg("WARNING: FAILED TO INSTANTIATE " + (std::string)objN.type + "\n");
			}
			else {
				insertObject(obj);
			}
		}
	}
}


std::vector<objNode>* ObjectManager::getObjectNodes() {
	nodes.clear();
	//for (std::vector<std::unique_ptr<GameObject>>::const_iterator obj = objects.begin(); obj != objects.end(); obj++)
	for (const auto& obj : objects)
	{
		if (obj.get()->hasObjectNode()) {
			nodes.push_back(obj.get()->getObjectNode());

			if (obj.get()->isTemplateUser()) {
				TemplateUser* temp = (TemplateUser*)obj.get()->getTemplateUser();
				nodes.push_back(temp->getTemplateNode());
			}
		}
	}

	return &nodes;
}

/*
void ObjectManager::applyLevelTriggers(GameObject *obj) {
	std::vector<Level::Trigger> *triggers = gameLevel->getTriggers();
	for (std::vector<Level::Trigger>::const_iterator it = triggers->begin(); it != triggers->end(); it++) {
		if (obj->getCollision().intersects(it->area)) {
			if (it->type == Level::TriggerType::DAMAGE) {
				//obj->hurt(Level::spikeDamage);
			}
			else if (it->type == Level::TriggerType::PUSH_LEFT) {
				//do nothing for now
			}
			else if (it->type == Level::TriggerType::PUSH_RIGHT) {
				//do nothing for now
			}
		}
	}
}
*/
