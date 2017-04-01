#ifndef TRANSITIONSTATE_H
#define TRANSITIONSTATE_H

#include <SFML/System.hpp>

#include "GameplayState.hpp"
#include "TileLayer.hpp"

#include "Math.hpp"

class TransitionState : public State {
public:
	TransitionState(GameplayState *game, Transition *activatedTransition, ResourceLoader *r);
	~TransitionState();

	void update(sf::Time deltaTime);
	
protected:

	void initNextObjects();

	float offsetIntoLevel;

	//useful pointer to gamestate objects
	GameplayState* gameplayState = nullptr;
	Zone *zone = nullptr;
	ObjectManager *objMan = nullptr;

	Player *plr = nullptr;
	std::vector<Effect*> parentedEffects;

	Zone::LevelArea *toLevelArea;
	Transition transition;

	//transition timing
	sf::Time transTimer;
	const static sf::Time transDuration;
	float fract;
	
	// transition positioning
	sf::Vector2f plrStart;
	sf::Vector2f plrEnd;
	sf::Vector2f camStart;
	sf::Vector2f camEnd;
	sf::Vector2f toLvlOffset;

	//fake level struct is used in case transition leads to the same level
	bool isFake = false;
	struct FakeLevel {
		FakeLevel(Level* lvl) :
		decorLayer(lvl->getResources(), lvl->getTilesetList()),
		foreLayer(lvl->getResources(), lvl->getTilesetList()),
		backLayer(lvl->getResources(), lvl->getTilesetList()) {

			levelArea = lvl->levelArea;

			decorLayer.copy(*lvl->getDecorativeLayer());
			foreLayer.copy(*lvl->getForegroundLayer());
			backLayer.copy(*lvl->getBackgroundLayer());

			for (auto i = lvl->getParallaxLayers()->begin(); i != lvl->getParallaxLayers()->end(); i++) {
				parallaxLayers.insert( std::make_pair(i->first, TileLayer( lvl->getResources(), lvl->getTilesetList() )) );
				parallaxLayers.at(i->first).copy(i->second);
			}
		}

		TileLayer decorLayer;
		TileLayer foreLayer;
		TileLayer backLayer;
		std::map<int, TileLayer> parallaxLayers;

		sf::IntRect levelArea;
	};
	void updateFakeLevel(sf::Time deltaTime, sf::FloatRect displayArea, sf::Vector2f camCenter);
	
	// used only if transitioning to the same level we're in
	FakeLevel *fakeLevel;

	virtual void updateBGSprites() = 0;

	//covers up non level areas
	std::vector<sf::RectangleShape> covers;
	virtual void initCovers(const sf::FloatRect &offsetArea) = 0;

	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	virtual void drawBG(sf::RenderTarget &target, sf::RenderStates states) const = 0;
};

#endif