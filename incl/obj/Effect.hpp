#ifndef EFFECT_H
#define EFFECT_H

#include "res/ResourceLoader.hpp"
#include "obj/Entity.hpp"

class Effect : public Entity {
	//display an animation until the animation completes, then die
public:

	enum EffectType : int {
		DEATH_GENERIC,
		HIT_GENERIC,
		COIN_PICKUP,
		SPRINT_FX,
		EXPLOSION
	};

	enum EffectLayer : int {
		OVER,
		UNDER,
	};

	int layer = OVER;

	// Generic constuctor of predefined effect types
	Effect();

	// Constuctor, with animation to play and texture to use
	Effect(std::string texFile, std::string ani, int onLayer, bool flip);

	void update(sf::Time deltaTime);

	void setParent(Entity *p, sf::Vector2f off = sf::Vector2f());
	Entity* getParent();

	bool sfxWhite = false;

protected:
	Entity *parent = nullptr;
	sf::Vector2f parentOffset;

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
		//draw sprite
		target.draw(sprite, (!sfxWhite ? states : RL()->getShader("shaders/whiteout.frag")));
	
	};

	Animation anim;
};

#endif
