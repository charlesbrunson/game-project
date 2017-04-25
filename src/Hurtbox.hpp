#ifndef HURTBOX_H
#define HURTBOX_H

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "GameObject.hpp"

//for hurting things with projectiles/attacks
class Hurtbox : public sf::FloatRect, public sf::Drawable {
public:
	Hurtbox() {
		left = 0;
		top = 0;
		width = 0;
		height = 0;
	}

	void setCenter(sf::Vector2f pos);

	bool tryHurt(GameObject *obj);

	void update();

	void setActive(bool i);
	bool getActive() const;

	void clearIgnoreList();

	//how much damage to do when we hurt something
	int damage = 1;
	sf::Time stunTime = sf::Time::Zero;

private:
	bool isActive = false;

	//things we've hurt already
	std::vector<GameObject*> ignoreObjs;
	
	void updateIgnoreList();

	//draw for debugging purposes
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {

		sf::RectangleShape box;

		box.setPosition(left,top);
		box.setSize(sf::Vector2f(width, height));
		box.setFillColor(sf::Color(255, 0, 0, 100));

		target.draw(box, states);
	};
};



#endif