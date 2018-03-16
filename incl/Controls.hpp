#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#include <SFML/Window.hpp>

#include "util/Math.hpp"

class Controls {
public:
	struct JoystickInput {
		// Default constructor, is unbound
		JoystickInput() {
			isButton = false;
			isAxis = false;

			buttonNum = 0;
			axis = sf::Joystick::Axis::X;
			activeOnAxisPositive = true;
		}
		// Joystick button constructor
		JoystickInput(int number) {
			isButton = true;
			isAxis = false;

			buttonNum = number;
		}
		// Joystick axis constructor
		JoystickInput(sf::Joystick::Axis joyAxis, bool positiveActivation) {
			isButton = false;
			isAxis = true;

			axis = joyAxis;
			activeOnAxisPositive = positiveActivation;
		}

		bool isButton;
		int buttonNum;

		bool isAxis;
		sf::Joystick::Axis axis;
		bool activeOnAxisPositive;

	};
	struct GameInput {
		bool active = false;
		bool confirmed = false;
	};
	struct MouseInput {
		GameInput input;

		// Where mouse press _STARTED_ in game world coordinates
		// use Controls::mousePosition to track mouse movement instead
		sf::Vector2f position;
	};

	enum Input : int {
		UP = 0,
		RIGHT,
		DOWN,
		LEFT,
		JUMP,
		ATTACK,
		SPRINT,
		START,
		SELECT,
		INPUT_COUNT
	};

	//map cardinal directions to inputs
	static const Controls::Input dirMap[Cardinal::DIR_COUNT];
	
	// Keyboard controls
	static sf::Keyboard::Key UpK;
	static sf::Keyboard::Key RightK;
	static sf::Keyboard::Key DownK;
	static sf::Keyboard::Key LeftK;
	static sf::Keyboard::Key JumpK;
	static sf::Keyboard::Key AttackK;
	static sf::Keyboard::Key SprintK;
	static sf::Keyboard::Key StartK;
	static sf::Keyboard::Key SelectK;
	static sf::Keyboard::Key* const keyboardKeys[Input::INPUT_COUNT];

	static sf::Keyboard::Key FullscreenToggle;


	// Joystick controls
	static JoystickInput UpJ;
	static JoystickInput RightJ;
	static JoystickInput DownJ;
	static JoystickInput LeftJ;
	static JoystickInput JumpJ;
	static JoystickInput AttackJ;
	static JoystickInput SprintJ;
	static JoystickInput StartJ;
	static JoystickInput SelectJ;
	static JoystickInput* const joystickKeys[Input::INPUT_COUNT];

	// joystick input deadzone
	static float deadZone;
	

	// Mouse input
	static sf::Vector2f mousePosition;
	static bool mouseInWindow;
	static MouseInput mouseActive;
	static sf::Time mouseLastMoved;


	// Input consolodated across joystick and keyboard
	static GameInput UpActive;
	static GameInput RightActive;
	static GameInput DownActive;
	static GameInput LeftActive;
	static GameInput JumpActive;
	static GameInput AttackActive;
	static GameInput SprintActive;
	static GameInput StartActive;
	static GameInput SelectActive;
	static GameInput* const inputs[Input::INPUT_COUNT];

	// Input state access
	static bool isPressed(int input) {
		return inputs[input]->active && !inputs[input]->confirmed;
	}
	static bool isHeld(int input) {
		return inputs[input]->active && inputs[input]->confirmed;
	}
	static void confirmPress(int input) {
		inputs[input]->confirmed = true;
	}
	static std::vector<int> isAnyPressed(const std::vector<int> &inputs) {
		std::vector<int> r;
		for (int i : inputs) {
			if (isPressed(i)) {
				r.push_back(i);
			}
		}
		return r;
	}

	//mouse input
	static bool isMousePressed() {
		return mouseActive.input.active && !mouseActive.input.confirmed;
	}
	static bool isMouseHeld() {
		return mouseActive.input.active && mouseActive.input.confirmed;
	}
	static void confirmedMousePress() {
		mouseActive.input.confirmed = true;
	}
	static sf::Vector2f getMousePressPosition() {
		return mouseActive.position;
	}

	static void updateGameInputs() {
		for (int i = 0; i < Input::INPUT_COUNT; i++) {
			if (!inputs[i]->active) {
				inputs[i]->confirmed = false;
			}
		}
		if (!mouseActive.input.active) {
			mouseActive.input.confirmed = false;
		}
	}

	// Force depress of all active inputs
	static void clearGameInputs() {
		for (int i = 0; i < Input::INPUT_COUNT; i++) {
			inputs[i]->confirmed = true;
		}
		mouseActive.input.confirmed = true;
	}

	// Association for input keys and their labels
	const static std::pair<std::string, sf::Keyboard::Key> keyboardKeyStrings[sf::Keyboard::KeyCount];
	const static std::pair<std::string, int> joystickButtons[sf::Joystick::ButtonCount];
	const static std::pair<std::string, sf::Joystick::Axis> joystickAxis[sf::Joystick::AxisCount];
};

#endif
