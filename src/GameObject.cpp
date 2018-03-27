#include <cfloat>
#include <math.h>

#include "obj/GameObject.hpp"

void GameObject::setCollision(sf::FloatRect col) {
	collisionBox = col;
};
sf::FloatRect GameObject::getCollision() {
	return collisionBox;
};

void GameObject::setPosition(sf::Vector2f pos, bool updateOldBox) {
	Entity::setPosition(pos, updateOldBox);



	if (updateOldBox)
		oldBox = collisionBox;

	setCollisionPosition(position);
};


void GameObject::setLevel(Level *level) {
	lvl = level;
};
Level * GameObject::getLevel(){
	return lvl;
};


void GameObject::createObject(GameObject* obj) {
	createdObjs.push_back(obj);
};
void GameObject::createEffect(Effect* fx) {
	createdEffects.push_back(fx);
};
void GameObject::createSound(std::string soundFile) {
	createdSounds.push_back(soundFile);
};

//really just for Player use, but who knows?
bool GameObject::canDropThroughFloor() {

	float targetPlatY = FLT_MAX;
	float currentY;

	bool gotCurrentPlat = false;

	if (!onGround)
		return false;

	//get closest upplat below us = TargetPlatY
	for (sf::FloatRect upPlat : *lvl->getUpCol()) {

		//on same height
		if (upPlat.top == position.y) {

			//actually under us
			if (collisionBox.left < upPlat.left + upPlat.width && collisionBox.left + collisionBox.width > upPlat.left) {
				currentY = upPlat.top;
				gotCurrentPlat = true;
				break;
			}
		}
	}

	//cycle through other plattypes, ensure none are between currentY and targetPlatY
	if (gotCurrentPlat) {

		//find next closest platform height
		for (sf::FloatRect upPlat : *lvl->getUpCol()) {
			if (collisionBox.left < upPlat.left + upPlat.width && collisionBox.left + collisionBox.width > upPlat.left) {
				if (upPlat.top > currentY) {
					targetPlatY = fminf(upPlat.top, targetPlatY);
				}
			}
		}

		sf::FloatRect dropBox;
		dropBox.height = targetPlatY - currentY;
		dropBox.width = collisionBox.width;
		dropBox.left = collisionBox.left;
		dropBox.top = currentY;

		if (dropBox.top + dropBox.height > lvl->levelArea.top + lvl->levelArea.height) {
			return false;
		}

		for (sf::FloatRect plat : *lvl->getDownCol()) {
			if (dropBox.intersects(plat)) {
				return false;
			}
		}
		for (sf::FloatRect plat : *lvl->getLeftCol()) {
			if (dropBox.intersects(plat)) {
				return false;
			}
		}
		for (sf::FloatRect plat : *lvl->getRightCol()) {
			if (dropBox.intersects(plat)) {
				return false;
			}
		}
		return true;
	}
	return false;
}

void GameObject::setCollisionSize(float width, float height, bool centerOrigin) {
	collisionBox.width = width;
	collisionBox.height = height;

	colOffset.x = -collisionBox.width / 2.f;
	if (centerOrigin)
		colOffset.y = -collisionBox.height / 2.f;
	else
		colOffset.y = -collisionBox.height;
}

void GameObject::setCollisionPosition(sf::Vector2f p) {
	collisionBox.left = p.x + colOffset.x;
	collisionBox.top = p.y + colOffset.y;
};

sf::Vector2f GameObject::getCollisionPosition() {
	return sf::Vector2f(collisionBox.left - colOffset.x, collisionBox.top - colOffset.y);
};

void GameObject::resetCollisionFlags() {
	onGround = false;
	droppingThroughFloor = false;
	collisionUp = false;
	collisionRight = false;
	collisionDown = false;
	collisionLeft = false;
}

void GameObject::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	if (!doNotDraw) {
		if (sfxFlashing) {
			//roughly 30fps in ms
			int frameTime = (int)(2.0 / 60.0 * 1000.0);

			//alternate drawing every (frameTime) ms
			if (lifeTime.asMilliseconds() % (frameTime * 2) >= frameTime) {
				//don't draw this frame
				return;
			}
		}

		target.draw(sprite, (!sfxWhite ? states : RL()->getShader("shaders/whiteout.frag")));
	}
#ifdef _DEBUG
	if (Gameplay_Globals::Debug::objectCollision) {

		sf::VertexArray colVert(sf::LinesStrip, 5);
		colVert[0] = sf::Vertex(Math::topleft(collisionBox), sf::Color::Green);
		colVert[1] = sf::Vertex(Math::topright(collisionBox), sf::Color::Green);
		colVert[2] = sf::Vertex(Math::bottomright(collisionBox), sf::Color::Green);
		colVert[3] = sf::Vertex(Math::bottomleft(collisionBox), sf::Color::Green);
		colVert[4] = sf::Vertex(Math::topleft(collisionBox), sf::Color::Green);

		sf::VertexArray oColVert(sf::LinesStrip, 5);
		oColVert[0] = sf::Vertex(Math::topleft(oldBox), sf::Color::Blue);
		oColVert[1] = sf::Vertex(Math::topright(oldBox), sf::Color::Blue);
		oColVert[2] = sf::Vertex(Math::bottomright(oldBox), sf::Color::Blue);
		oColVert[3] = sf::Vertex(Math::bottomleft(oldBox), sf::Color::Blue);
		oColVert[4] = sf::Vertex(Math::topleft(oldBox), sf::Color::Blue);

		sf::RectangleShape posRect;

		target.draw(oColVert, states);

		//place rectshape over collision rect and draw it
		target.draw(colVert, states);

		posRect.setPosition(position + sf::Vector2f(-0.5f, -0.5f));
		posRect.setSize(sf::Vector2f(1, 1));
		posRect.setFillColor(sf::Color(255, 0, 0, 255));

		target.draw(posRect, states);
	}
#endif
}
