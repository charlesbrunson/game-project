#ifndef ENGINE_H
#define ENGINE_H

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "State.hpp"
#include "GLog.hpp"
#include "Controls.hpp"

// Game window and input handler
class Engine : public sf::RenderWindow {
public:
	Engine();
	~Engine();
		
	// Window camera
	sf::View view;

	// Resource loader and handler
	//ResourceLoader resources;

	// Active state object
	State *activeState = nullptr;
	
	// Commences game loop
	void run();

	// Applies settings from GameSettings class
	void applySettings();

	// Recreates window as fullscreen or windowed
	void goFullscreen(bool set);

	// Updates active state
	void update(sf::Time t);
	void render();
	
private:

	// GameLog containing recent debug messages
	GLog gLog;

	// Number of frames processed since program start
	unsigned long long int _frameCounter = 0;


	// Flag indicating if window is fullscreen or windowed mode
	bool _fullscreen = false;
	// Flag indicating to toggle window's fullscreen state
	bool _toToggleFullscreen = false;
	// Flag to allow unused margins in a window, or to auto resize to remove them
	bool _removeWindowMargins = false;


	// This window is currently in focus
	bool _focus = true;


	// Toggles for VSync for both fullscreen and windowed mode
	bool _windowedVSync = false;
	bool _fullscreenVSync = false;


	// Time between each update
	sf::Time _timePerUpdate;
	// Time between each draw call
	sf::Time _timePerDraw;
	// Maximum time allowed between draw calls, more than this and the game will begin to noticable slow down to compensate
	sf::Time _minUpdateSpeed = sf::milliseconds(33); //60fps


	// Window title
	const std::string _gameName = "Untitled";


	// Update rate cap
	double _maxUpdatePerSec;
	// Draw call rate cap
	double _maxDrawPerSec;
	// Flag indicating if the updates/draws should be uncapped
	// If true, game will use as much processing speed as available
	bool _uncappedFPS;


#ifdef _DEBUG
	// FPS tracking
	// Frame counters
	int _dFrameCount;
	int _dUpdateCount;
	// Calculated FPS
	float _dDrawFPS;
	float _dUpdateFPS;
	// FPS tracker
	sf::Clock _dFpsTracker;
#endif
	
	// Gets associated Key enum based on string value
	sf::Keyboard::Key getKey(std::string k);
	// Gets associated JoystickInput based on string value
	Controls::JoystickInput getJoystickInput(std::string j);
	

	// Window size and position on screen, when in windowed mode
	// Used for restoring size/pos when changing from windowed to fullscreen and back again
	sf::Vector2i _windowedSize;
	sf::Vector2i _windowedPos;


	// Game scalar, to scaling the normally quite small game area to fit larger windows
	unsigned int _gameScale = 1;

	// Guessed value for ideal window scale based on screen size when in windowed mode
	int _initialScale;

	// Tracks passed time, for determining when to update/draw
	sf::Clock _gameClock;
	sf::Time _timeSinceLastUpdate;
	sf::Time _timeSinceLastDraw;
	

	// Interprets window events
	void handleEvents();

	// Updates game scale to fit resized window
	void resizeWindow(int width, int height);

	// sf::RenderWindow method, redirecting to resizeWindow
	void onResize() { resizeWindow(getSize().x, getSize().y); };

	// Recursively destroys state and states it contains in a recursive fashion
	void destroyState(State *st) {
		if (st) {
			if (st->hasNextState()) {
				destroyState(st->getNextState());
				st->setNextState(nullptr);
			}

			delete st;
		}
	}
};

#endif 
