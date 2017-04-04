
#include <assert.h>
#include <thread>

#include "Game.hpp"
#include "ResourceLoader.hpp"

#include "MusicPlayer.hpp"
#include "GameSettings.hpp"

#include "UIMainMenu.hpp"
#include "GameplayState.hpp"


// Constructor
Game::Game() {

	// Start an instance of GameplayState for our starting state
	// Ideally this should be changed to a main menu or something
	activeState = new GameplayState();
	//activeState = new UIMainMenu();

	// Setup default FPS settings
	_maxUpdatePerSec = 144.0;
	_maxDrawPerSec = 144.0;
	_uncappedFPS = true;

#ifdef _DEBUG
	// Setup debug values
	_dFrameCount = 0;
	_dUpdateCount = 0;
	_dDrawFPS = 0;
	_dUpdateFPS = 0;
#endif
	
	// Read settings file and apply it
	applySettings();

	// Setup update/draw rates based on FPS caps
	_timePerUpdate = sf::seconds(static_cast<float>(1.0 / _maxUpdatePerSec));
	_timePerDraw = sf::seconds(static_cast<float>(1.0 / _maxDrawPerSec));

	// Setup window scale
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	{
		_initialScale = (int)std::min(floor((float)desktop.width / GAMEWIDTH), floor((float)(desktop.height - 64) / GAMEHEIGHT));

		// Make window a bit small after a certain threshold
		if (_initialScale > 4)
			_initialScale--;

		_windowedSize = sf::Vector2i(GAMEWIDTH * _initialScale, GAMEHEIGHT * _initialScale);
	}
	// Create game window
	create(sf::VideoMode(_windowedSize.x, _windowedSize.y, desktop.bitsPerPixel), _gameName, sf::Style::Default);

	// Set window properties
	setActive(false);
	setFramerateLimit(0);
	setKeyRepeatEnabled(false);

	// Resize window to calculated scale
	resizeWindow(GAMEWIDTH * _initialScale, GAMEHEIGHT * _initialScale);

	// Setup window position
	_windowedPos = getPosition();
}

// Deconstructor
Game::~Game() {

	// Eliminates active states
	destroyState(activeState);

}


// Main game loop
void Game::run() {

#ifdef _DEBUG
	std::string fpsLabel = "FPS";
	Log::track(fpsLabel, std::to_string((int)_dDrawFPS));
#endif

	// Time elapsed on gameclock since last restart
	sf::Time elapsed = sf::Time::Zero;

	// Time accumulators
	_timeSinceLastUpdate = sf::Time::Zero;
	_timeSinceLastDraw = sf::Time::Zero;

	// Reset clock before loop gets started proper
	_gameClock.restart();

	// Flag to end loop and begin closing game
	bool stop = false;

	// Start music manager thread
	std::thread musicThread = std::thread(MusicPlayer::run);

	// Commence game loop
	while (isOpen() && !stop && activeState) {

		// Catch input events
		sf::Clock t;
		handleEvents();

		if (_focus) {
			if (t.getElapsedTime() > _timePerUpdate)
				Log::msg("check " + std::to_string(t.restart().asMicroseconds()) + "\n");

			elapsed = _gameClock.restart();

			_timeSinceLastUpdate += elapsed;
			_timeSinceLastDraw += elapsed;

			bool toSleep = false;

			// Step 1: Update state
			if (_timeSinceLastUpdate >= _timePerUpdate || _uncappedFPS) {

				if (_uncappedFPS) {
					// If uncapped, just use elapsed time as delta time

					Controls::updateGameInputs();
					sf::Time dt = std::min(_timeSinceLastUpdate, _minUpdateSpeed);
					update(dt);
					Controls::mouseLastMoved += dt;

					// Clear debug log
					//ClearLog::msg();

#ifdef _DEBUG
					//Log::msg("FPS:  " + std::to_string((int)_dDrawFPS) + (_dDrawFPS < 100 ? "\t\t" : "\t") + "UPS:  " + std::to_string((int)_dUpdateFPS) + "\n");
#endif

					// Reset update time accumulator
					_timeSinceLastUpdate = sf::Time::Zero;
				}
				else {
					while (_timeSinceLastUpdate >= _timePerUpdate) {
						// If capped, update at set interval until caught up

						Controls::updateGameInputs();
						update(_timePerUpdate);
						Controls::mouseLastMoved += _timePerUpdate;

						// Clear debug log
						//ClearLog::msg();

#ifdef _DEBUG
						//Log::msg("FPS:  " + std::to_string((int)_dDrawFPS) + (_dDrawFPS < 100 ? "\t\t" : "\t") + "UPS:  " + std::to_string((int)_dUpdateFPS) + "\n");
#endif

						// Reset update time accumulator
						_timeSinceLastUpdate -= _timePerUpdate;
					}
				}
				// Increment frames processed
				_frameCounter++;
				
				// Activate stop flag if active state wants to end program
				if (activeState->quitGame) {
					stop = true;
				}

#ifdef _DEBUG
				// Increment update counter
				_dUpdateCount++;
#endif

			}

			// Step 2: Draw state
			if (_timeSinceLastDraw >= _timePerDraw || _uncappedFPS) {
				render();

				if (_uncappedFPS) {
					_timeSinceLastDraw = sf::Time::Zero;
				}
				else {
					sf::Time t = std::min(_timePerDraw, _minUpdateSpeed);
					_timeSinceLastDraw %= t;
				}
#ifdef _DEBUG
				// Increment draw counter
				_dFrameCount++;
#endif
			}

#ifdef _DEBUG
			// Update FPS tracking 
			if (_dFpsTracker.getElapsedTime().asSeconds() >= 1.0f) {
				_dFpsTracker.restart();
				_dDrawFPS = _dFrameCount;
				_dUpdateFPS = _dUpdateCount;
				_dFrameCount = 0;
				_dUpdateCount = 0;
				Log::updateValue(fpsLabel, std::to_string((int)_dDrawFPS));
			}
#endif
			// Step 3: Sleep (if necessary)
			if (_timePerDraw > _timeSinceLastDraw && _timePerUpdate > _timeSinceLastUpdate && !_uncappedFPS) {

				// Determine time until next update or draw
				sf::Time sleepTime = std::min(_timePerDraw - _timeSinceLastDraw, _timePerUpdate - _timeSinceLastUpdate);

				if (sleepTime > sf::Time::Zero) {
					// Sleep game loop until needed
					sf::sleep(sleepTime);
				}
			}
		}
		else {
			// This window is out of focus, sleep for another frame
			// This causes game to not update while out of focus
			sf::sleep(sf::seconds(std::min(1.f / _maxUpdatePerSec, 1.f / _maxDrawPerSec)));
		}
	}

	// Join music manager
	MusicPlayer::stopRun();
	musicThread.join();

#ifdef _DEBUG
	Log::remove(fpsLabel);
#endif
}

// Loads and applies settings from file
void Game::applySettings() {

	// Load settings from file
	GameSettings::readSettingsFile();

	// Apply settings
	volumeGame = std::stoi(std::string(GameSettings::getSettingValue(SettingsField::GameVolume)));
	volumeMusic = std::stoi(std::string(GameSettings::getSettingValue(SettingsField::MusicVolume)));
	_windowedVSync = std::stoi(std::string(GameSettings::getSettingValue(SettingsField::WINDOWED_VSYNC))) != 0;
	_fullscreenVSync = std::stoi(std::string(GameSettings::getSettingValue(SettingsField::FULLSCREEN_VSYNC))) != 0;
	_removeWindowMargins = std::stoi(std::string(GameSettings::getSettingValue(SettingsField::WINDOWMARGIN))) == 0;

	// Apply keyboard controls
	for (int a = SettingsField::CONTROLS_UP; a != SettingsField::CONTROLS_START; a++) {
		sf::Keyboard::Key k = getKey(GameSettings::getSettingValue(static_cast<SettingsField>(a)));

		if (k != sf::Keyboard::Unknown) {
			switch (static_cast<SettingsField>(a)) {
			case SettingsField::CONTROLS_UP: Controls::UpK = k; break;
			case SettingsField::CONTROLS_DOWN: Controls::DownK = k; break;
			case SettingsField::CONTROLS_LEFT: Controls::LeftK = k; break;
			case SettingsField::CONTROLS_RIGHT: Controls::RightK = k; break;
			case SettingsField::CONTROLS_JUMP: Controls::JumpK = k; break;
			case SettingsField::CONTROLS_ATTACK: Controls::AttackK = k; break;
			case SettingsField::CONTROLS_DASH: Controls::SprintK = k; break;
			case SettingsField::CONTROLS_SELECT: Controls::SelectK = k; break;
			case SettingsField::CONTROLS_START: Controls::StartK = k; break;
			}
		}
	}

	// Apply joystick controls
	for (int a = SettingsField::JOYSTICK_UP; a != SettingsField::JOYSTICK_START; a++) {
		Controls::JoystickInput j = getJoystickInput(GameSettings::getSettingValue(static_cast<SettingsField>(a)));

		if (j.buttonNum != -1) {
			switch (a) {
			case SettingsField::JOYSTICK_UP: Controls::UpJ = j; break;
			case SettingsField::JOYSTICK_DOWN: Controls::DownJ = j; break;
			case SettingsField::JOYSTICK_LEFT: Controls::LeftJ = j; break;
			case SettingsField::JOYSTICK_RIGHT: Controls::RightJ = j; break;
			case SettingsField::JOYSTICK_JUMP: Controls::JumpJ = j; break;
			case SettingsField::JOYSTICK_ATTACK: Controls::AttackJ = j; break;
			case SettingsField::JOYSTICK_DASH: Controls::SprintJ = j; break;
			case SettingsField::JOYSTICK_SELECT: Controls::SelectJ = j; break;
			case SettingsField::JOYSTICK_START: Controls::StartJ = j; break;
			}
		}
	}
	
	// Rewrite settings file (reformats it to be nice and valid)
	GameSettings::writeSettingsFile();
}

sf::Keyboard::Key Game::getKey(std::string k) {

	// Format string
	std::string key = std::string(k);
	std::transform(key.begin(), key.end(), key.begin(), toupper);
	
	// Find key
	// TODO: convert to binary search, not super important though because this only gets
	// called a few times at the start of a program
	for (int i = 0; i < sf::Keyboard::KeyCount; i++) {

		if (key == std::string(Controls::keyboardKeyStrings[i].first))
			return Controls::keyboardKeyStrings[i].second;

	}

	// Input couldn't be found, unbind it
	Log::msg("Unknown key: " + key + "\n");
	return sf::Keyboard::Unknown;
}

Controls::JoystickInput Game::getJoystickInput(std::string j) {

	// Format string
	std::string in = std::string(j);
	std::transform(in.begin(), in.end(), in.begin(), toupper);

	// Find input, strings that start with > or < are joystick axes,
	// which refers to the direction that activates it
	if (in.at(0) == '<' || in.at(0) == '>') {

		//rest of the string
		std::string a = in.substr(1, in.length());

		for (int i = 0; i < sf::Joystick::AxisCount; i++) {
			if (a == Controls::joystickAxis[i].first) {
				return Controls::JoystickInput(Controls::joystickAxis[i].second, in.at(0) == '>');
			}
		}
	}
	// Otherwise the string refers to a button
	else {		
		for (int i = 0; i < sf::Joystick::ButtonCount; i++) {
			if (in == Controls::joystickButtons[i].first) {
				return Controls::JoystickInput(i);
			}
		}
	}

	// Input couldn't be found, unbind it
	Log::msg("Unknown joystick input: " + in + "\n");
	return Controls::JoystickInput(-1);
}


// Change window to fullscreen or windowed mode
void Game::goFullscreen(bool set) {

	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();

	_fullscreen = set;
	if (_fullscreen) {

		// Remember window size, will be use when reverting back to windowed mode from fullscreen
		_windowedSize = sf::Vector2i(getSize());
		_windowedPos = getPosition();

		// Create windowed fullscreen mode (because regular fullscreen is bad at alt-tabbing out)
		create(sf::VideoMode(desktop.width, desktop.height, 32), _gameName, sf::Style::None);
		resizeWindow(desktop.width, desktop.height);

		// Change VSync according to settings
		setVerticalSyncEnabled(_fullscreenVSync);

		// Set cursor invisible
		setMouseCursorVisible(false);
	}
	else {

		// Try to restore original windowed size
		sf::Vector2i size;
		if (_windowedSize.x > GAMEWIDTH * _initialScale || _windowedSize.y > GAMEHEIGHT * _initialScale)
			size = sf::Vector2i(GAMEWIDTH * _initialScale, GAMEHEIGHT * _initialScale);
		else
			size = _windowedSize;

		// Create window in windowed mode
		create(sf::VideoMode(size.x, size.y, 32), _gameName, sf::Style::Default);
		resizeWindow(size.x, size.y);

		// Mode window to original position
		setPosition(_windowedPos);

		// Change VSync according to settings
		setVerticalSyncEnabled(_windowedVSync);

		// Set cursor visible
		setMouseCursorVisible(true);
	}
}


// Update current states and run various game logic
void Game::update(sf::Time t) {

	// Update active state
	activeState->update(t);

	// State wants to move to another state
	if (activeState->toNextState){
		// Ensure state exists
		assert(activeState->hasNextState());

		// Switch state
		State *tempState = activeState->getNextState();

		if (activeState->removeStateOnChange) {
			// Remove old state
			delete activeState;
		}
		else {
			// Retain old state, but remove next state from it
			activeState->setNextState(nullptr);
			activeState->toNextState = false;
		}

		// Clear inputs so they don't get crossed over into next state
		// (ex: pressing A on pause state's resume button causing player to jump once resumed)
		Controls::clearGameInputs();

		// Assign new state to active state
		activeState = tempState;
	}

	// Update window's view
	view = getView();
	sf::Vector2f v = activeState->cameraPos;
	v = snapToPixel(v);
	sf::Vector2f prevCenter = v;
	view.setCenter(v);
	setView(view);
	if (Controls::mouseInWindow && prevCenter != view.getCenter()) {
		Controls::mousePosition += Math::diff(prevCenter, view.getCenter());
	}


#ifdef _DEBUG
	// Update ingame Log text
	gLog.update(v);
#endif

}

// Draw ative state
void Game::render() {

	// Clear window with active state's preferred color
	clear(activeState->clearColor);

	// Draw active state
	draw(*activeState);

#ifdef _DEBUG
	// Draw debug log
	draw(gLog);
#endif

	// Update window display with drawn objects
	display();
}

// Handle window events
void Game::handleEvents() {
	
	sf::Event e;
	while (pollEvent(e)) {
		switch (e.type) {

			// Window has lost focus
		case sf::Event::EventType::LostFocus:
			_focus = false;
			break;

			// Window has gained focus
		case sf::Event::EventType::GainedFocus:
			_focus = true;
			_gameClock.restart();
			break;

			// Window has been closed
		case sf::Event::EventType::Closed:
			close();
			break;

			// Window has been resized
		case sf::Event::EventType::Resized:
			//resizeWindow(e.size.width, e.size.height);
			break;

			// Keyboard Input
		case sf::Event::EventType::KeyPressed:
			for (int i = 0; i < Controls::INPUT_COUNT; i++) {
				if (e.key.code == *Controls::keyboardKeys[i]) {
					Controls::inputs[i]->active = true;
				}
			}
			break;

		case sf::Event::EventType::KeyReleased:
			for (int i = 0; i < Controls::INPUT_COUNT; i++) {
				if (e.key.code == *Controls::keyboardKeys[i]) {
					Controls::inputs[i]->active = false;
					break;
				}
			}
			break;

			// Joystick Input
		case sf::Event::EventType::JoystickButtonPressed:
			for (int i = 0; i < Controls::INPUT_COUNT; i++) {
				if (Controls::joystickKeys[i]->isButton && e.joystickButton.button == Controls::joystickKeys[i]->buttonNum) {
					Controls::inputs[i]->active = true;
					break;
				}
			}
			break;

		case sf::Event::EventType::JoystickButtonReleased:
			for (int i = 0; i < Controls::INPUT_COUNT; i++) {
				if (Controls::joystickKeys[i]->isButton && e.joystickButton.button == Controls::joystickKeys[i]->buttonNum) {
					Controls::inputs[i]->active = false;
					break;
				}
			}
			break;

		case sf::Event::EventType::JoystickMoved:
			// Figure out which input it was
			for (int i = 0; i < Controls::INPUT_COUNT; i++) {
				// Is this input assigned to the moved axis
				if (Controls::joystickKeys[i]->isAxis && e.joystickMove.axis == Controls::joystickKeys[i]->axis) {
					// Is axis past deadzone in this input's direction
					if (e.joystickMove.position != 0
						&& ((e.joystickMove.position > 0.f) == Controls::joystickKeys[i]->activeOnAxisPositive)
						&& abs(e.joystickMove.position) >= Controls::deadZone) {

						// enable input
						Controls::inputs[i]->active = true;

						// If another input is assigned to same axis but opposite direction, disable it
						for (int j = i; j < Controls::INPUT_COUNT; j++) {
							if (Controls::joystickKeys[j]->isAxis
								&& e.joystickMove.axis == Controls::joystickKeys[j]->axis == Controls::joystickKeys[i]->axis
								&& Controls::joystickKeys[j]->activeOnAxisPositive != Controls::joystickKeys[i]->activeOnAxisPositive) {

								Controls::inputs[i]->active = false;
								break;
							}
						}
						break;
					}
					else {
						Controls::inputs[i]->active = false;
					}
				}
			}
			break;

			// Mouse Input
		case sf::Event::MouseEntered:
			Controls::mouseInWindow = true;
			Controls::mouseLastMoved = sf::Time::Zero;
			break;

		case sf::Event::MouseLeft:
			Controls::mouseInWindow = false;
			Controls::mouseLastMoved = sf::Time::Zero;
			break;

		case sf::Event::MouseMoved:
			Controls::mousePosition = mapPixelToCoords(sf::Vector2i(e.mouseMove.x, e.mouseMove.y));
			Controls::mouseLastMoved = sf::Time::Zero;
			break;

		case sf::Event::MouseButtonPressed:
			if (getCameraArea().contains(Controls::mousePosition) && Controls::mouseInWindow
				&& e.mouseButton.button == sf::Mouse::Left || e.mouseButton.button == sf::Mouse::Right) {

				Controls::mouseActive.input.active = true;
				Controls::mouseActive.position = Controls::mousePosition;
			}
			break;

		case sf::Event::MouseButtonReleased:
			if (e.mouseButton.button == sf::Mouse::Left || e.mouseButton.button == sf::Mouse::Right) {
				Controls::mouseActive.input.active = false;
				Controls::mouseActive.position = Controls::mousePosition;
			}
			break;
		}
	}
	
	// Separated from other input events due to input issues when recreating the window (such as not catching input release)
	if (!_toToggleFullscreen && (sf::Keyboard::isKeyPressed(Controls::FullscreenToggle) ||
		(_fullscreen && sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)))) {

		goFullscreen(!_fullscreen);
		_toToggleFullscreen = true;

	}
	else if (_toToggleFullscreen && !sf::Keyboard::isKeyPressed(Controls::FullscreenToggle)) {
		_toToggleFullscreen = false;
	}
}

// Resize window view to best display contents
void Game::resizeWindow(int width, int height) {
	
	sf::Vector2u size;
	size.x = std::max(width, GAMEWIDTH);
	size.y = std::max(height, GAMEHEIGHT);

	// Is margins not allowed?
	if (_removeWindowMargins && !_fullscreen) {

		// Remove margins by resizing window
		int _scale = std::min(floorf(size.x / GAMEWIDTH), floorf(size.y / GAMEHEIGHT));
		sf::Vector2u nSize(_scale * GAMEWIDTH, _scale * GAMEHEIGHT);

		{
			//Move window
			sf::Vector2u move((size.x - nSize.x) / 2, (size.y - nSize.y) / 2);
			sf::Vector2i winP = getPosition() + sf::Vector2i(move);

			// Make sure window does not move off screen
			winP.y = std::min(winP.y, (int)sf::VideoMode::getDesktopMode().height - 64);

			setPosition(winP);
		}

		// Assign new size
		size = nSize;
	}

	// Resize window if new size is different from old size
	if (size.x != width || size.y != height)
		setSize(size);

	// Update the view to the new size of the window
	// Game view port scales automatically with window
	sf::FloatRect visibleArea(0, 0, GAMEWIDTH, GAMEHEIGHT);
	//sf::View view = sf::View(visibleArea);

	// Integer scaling method
	int scale = static_cast<int>(std::min(floor((double)size.x / GAMEWIDTH), floor((double)size.y / GAMEHEIGHT)));

	// Fit scaling method
	//float scale = std::min((float)size.x / GAMEWIDTH, (float)size.y / GAMEHEIGHT);

	// Fill scaling method
	//float scale = std::max((float)size.x / GAMEWIDTH, (float)size.y / GAMEHEIGHT);

	// Actual size of game display on screen
	double trueX = (double)GAMEWIDTH * scale;
	double trueY = (double)GAMEHEIGHT * scale;

	// Apply actual size to view, and zoom to compensate
	view.setSize((float)trueX, (float)trueY);
#ifdef _DEBUG
	if (!Gameplay_Globals::Debug::noZoom) {
		view.zoom(1.f / scale);
	}
	else {
		view.zoom(1.f);
	}
#else
	view.zoom(1.f / scale);
#endif
	
	// Offset view if there are margins
	double offX = ((double)size.x - trueX) / 2;
	double offY = ((double)size.y - trueY) / 2;

	// Apply offsets to viewport
	view.setViewport(
		sf::FloatRect(
		(float)offX / size.x,
		(float)offY / size.y,
		(float)trueX / size.x,
		(float)trueY / size.y)
		);

	// Update scale values
	_gameScale = scale;
	Scale::set(scale);

	// Update window view
	setView(view);
}