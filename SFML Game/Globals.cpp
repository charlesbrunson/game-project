#include "Globals.hpp"

const int GAMEWIDTH = 320; //20 tiles
const int GAMEHEIGHT = 240; //15 tiles

const sf::Vector2f GAMEDIMENSIONS(GAMEWIDTH, GAMEHEIGHT);

int volumeGame = 50;
int volumeMusic = 0;
float volumeMusicPausedScale = 0.5f;

int globalGameScale;

sf::Time Gameplay_Globals::minSoundTime = sf::seconds(1.f / 60.f);
sf::Time Gameplay_Globals::soundTimeout = sf::seconds(30.f / 60.f);

bool forcePackResources = true;
bool pixelSnapping = false;

bool Gameplay_Globals::Debug::objectCollision = false;
bool Gameplay_Globals::Debug::levelCollision = false;
bool Gameplay_Globals::Debug::platformPathing = false;
bool Gameplay_Globals::Debug::scrollingParallax = false;
bool Gameplay_Globals::Debug::noZoom = false;
bool Gameplay_Globals::Debug::playerInvulnerable = false;
bool Gameplay_Globals::Debug::forceCompile =
#ifdef _DEBUG 
true;
#else
false;
#endif


bool UI_Globals::Debug::UIDebug = true;
bool UI_Globals::Debug::UIConnectors = true;

int Scale::scale = 1;
