#include "Globals.hpp"

constexpr int GAMEWIDTH = 320; //20 tiles
constexpr int GAMEHEIGHT = 240; //15 tiles

const sf::Vector2f GAMEDIMENSIONS(GAMEWIDTH, GAMEHEIGHT);

int volumeGame = 50;
int volumeMusic = 0;
float volumeMusicPausedScale = 0.5f;

const sf::Time Gameplay_Globals::minSoundTime = sf::seconds(1.f / 60.f);
const sf::Time Gameplay_Globals::soundTimeout = sf::seconds(30.f / 60.f);

int Gameplay_Globals::gameScale = 1;

//bool forcePackResources = true;
bool pixelSnapping = false;

bool Gameplay_Globals::Debug::objectCollision = true;
bool Gameplay_Globals::Debug::levelCollision = true;
bool Gameplay_Globals::Debug::platformPathing = false;
bool Gameplay_Globals::Debug::scrollingParallax = false;
bool Gameplay_Globals::Debug::noZoom = false;
bool Gameplay_Globals::Debug::playerInvulnerable = false;

const bool Resource_Globals::ignorePackFile = true;

bool UI_Globals::Debug::UIDebug = true;
bool UI_Globals::Debug::UIConnectors = true;
