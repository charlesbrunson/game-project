#ifndef CAMREF_H
#define CAMREF_H

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
//#include "Globals.hpp"

//#include "GameObject.hpp"

//global variable containing copy of game camera's center
extern sf::Vector2f gCameraPos;
//global variable containing copy of player's position
//extern sf::Vector2f gPlayerPos;

//extern GameObject* gPlayer;

extern const sf::FloatRect getCameraArea();


#endif