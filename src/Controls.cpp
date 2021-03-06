#include "Controls.hpp"

//these are the defaults, could be overwritten by something
sf::Keyboard::Key Controls::FullscreenToggle = sf::Keyboard::F9;

//player controls
sf::Keyboard::Key Controls::UpK = sf::Keyboard::W;
sf::Keyboard::Key Controls::RightK = sf::Keyboard::D;
sf::Keyboard::Key Controls::DownK = sf::Keyboard::S;
sf::Keyboard::Key Controls::LeftK = sf::Keyboard::A;

sf::Keyboard::Key Controls::JumpK = sf::Keyboard::Space;
sf::Keyboard::Key Controls::AttackK = sf::Keyboard::K;
sf::Keyboard::Key Controls::SprintK = sf::Keyboard::LShift;

sf::Keyboard::Key Controls::StartK = sf::Keyboard::Return;
sf::Keyboard::Key Controls::SelectK = sf::Keyboard::RShift;

sf::Keyboard::Key* const Controls::keyboardKeys[Controls::Input::INPUT_COUNT] = {
	&Controls::UpK,
	&Controls::RightK,
	&Controls::DownK,
	&Controls::LeftK,
	&Controls::JumpK,
	&Controls::AttackK,
	&Controls::SprintK,
	&Controls::StartK,
	&Controls::SelectK
};

Controls::JoystickInput Controls::UpJ;
Controls::JoystickInput Controls::RightJ;
Controls::JoystickInput Controls::DownJ;
Controls::JoystickInput Controls::LeftJ;

Controls::JoystickInput Controls::JumpJ;
Controls::JoystickInput Controls::AttackJ;
Controls::JoystickInput Controls::SprintJ;

Controls::JoystickInput Controls::StartJ;
Controls::JoystickInput Controls::SelectJ;

Controls::JoystickInput* const Controls::joystickKeys[Controls::Input::INPUT_COUNT] = {
	&Controls::UpJ,
	&Controls::RightJ,
	&Controls::DownJ,
	&Controls::LeftJ,
	&Controls::JumpJ,
	&Controls::AttackJ,
	&Controls::SprintJ,
	&Controls::StartJ,
	&Controls::SelectJ
};
float Controls::deadZone = 25.f;

Controls::GameInput Controls::UpActive;
Controls::GameInput Controls::LeftActive;
Controls::GameInput Controls::DownActive;
Controls::GameInput Controls::RightActive;
Controls::GameInput Controls::JumpActive;
Controls::GameInput Controls::AttackActive;
Controls::GameInput Controls::SprintActive;
Controls::GameInput Controls::StartActive;
Controls::GameInput Controls::SelectActive;

Controls::GameInput* const Controls::inputs[Controls::Input::INPUT_COUNT] {
	&Controls::UpActive,
	&Controls::RightActive,
	&Controls::DownActive,
	&Controls::LeftActive,
	&Controls::JumpActive,
	&Controls::AttackActive,
	&Controls::SprintActive,
	&Controls::StartActive,
	&Controls::SelectActive
};
const Controls::Input Controls::dirMap[Cardinal::DIR_COUNT] {
	UP,
	RIGHT,
	DOWN,
	LEFT
};


// Mouse input
sf::Vector2f Controls::mousePosition;
bool Controls::mouseInWindow;
Controls::MouseInput Controls::mouseActive;
sf::Time Controls::mouseLastMoved = sf::Time::Zero;


const std::pair<std::string, sf::Keyboard::Key> Controls::keyboardKeyStrings[sf::Keyboard::KeyCount] =
{
	std::pair<std::string, sf::Keyboard::Key>("A", sf::Keyboard::A),
	std::pair<std::string, sf::Keyboard::Key>("B", sf::Keyboard::B),
	std::pair<std::string, sf::Keyboard::Key>("C", sf::Keyboard::C),
	std::pair<std::string, sf::Keyboard::Key>("D", sf::Keyboard::D),
	std::pair<std::string, sf::Keyboard::Key>("E", sf::Keyboard::E),
	std::pair<std::string, sf::Keyboard::Key>("F", sf::Keyboard::F),
	std::pair<std::string, sf::Keyboard::Key>("G", sf::Keyboard::G),
	std::pair<std::string, sf::Keyboard::Key>("H", sf::Keyboard::H),
	std::pair<std::string, sf::Keyboard::Key>("I", sf::Keyboard::I),
	std::pair<std::string, sf::Keyboard::Key>("J", sf::Keyboard::J),
	std::pair<std::string, sf::Keyboard::Key>("K", sf::Keyboard::K),
	std::pair<std::string, sf::Keyboard::Key>("L", sf::Keyboard::L),
	std::pair<std::string, sf::Keyboard::Key>("M", sf::Keyboard::M),
	std::pair<std::string, sf::Keyboard::Key>("N", sf::Keyboard::N),
	std::pair<std::string, sf::Keyboard::Key>("O", sf::Keyboard::O),
	std::pair<std::string, sf::Keyboard::Key>("P", sf::Keyboard::P),
	std::pair<std::string, sf::Keyboard::Key>("Q", sf::Keyboard::Q),
	std::pair<std::string, sf::Keyboard::Key>("R", sf::Keyboard::R),
	std::pair<std::string, sf::Keyboard::Key>("S", sf::Keyboard::S),
	std::pair<std::string, sf::Keyboard::Key>("T", sf::Keyboard::T),
	std::pair<std::string, sf::Keyboard::Key>("U", sf::Keyboard::U),
	std::pair<std::string, sf::Keyboard::Key>("V", sf::Keyboard::V),
	std::pair<std::string, sf::Keyboard::Key>("W", sf::Keyboard::W),
	std::pair<std::string, sf::Keyboard::Key>("X", sf::Keyboard::X),
	std::pair<std::string, sf::Keyboard::Key>("Y", sf::Keyboard::Y),
	std::pair<std::string, sf::Keyboard::Key>("Z", sf::Keyboard::Z),
	std::pair<std::string, sf::Keyboard::Key>("NUM0", sf::Keyboard::Num0),
	std::pair<std::string, sf::Keyboard::Key>("NUM1", sf::Keyboard::Num1),
	std::pair<std::string, sf::Keyboard::Key>("NUM2", sf::Keyboard::Num2),
	std::pair<std::string, sf::Keyboard::Key>("NUM3", sf::Keyboard::Num3),
	std::pair<std::string, sf::Keyboard::Key>("NUM4", sf::Keyboard::Num4),
	std::pair<std::string, sf::Keyboard::Key>("NUM5", sf::Keyboard::Num5),
	std::pair<std::string, sf::Keyboard::Key>("NUM6", sf::Keyboard::Num6),
	std::pair<std::string, sf::Keyboard::Key>("NUM7", sf::Keyboard::Num7),
	std::pair<std::string, sf::Keyboard::Key>("NUM8", sf::Keyboard::Num8),
	std::pair<std::string, sf::Keyboard::Key>("NUM9", sf::Keyboard::Num9),
	std::pair<std::string, sf::Keyboard::Key>("ESCAPE", sf::Keyboard::Escape),
	std::pair<std::string, sf::Keyboard::Key>("LCONTROL", sf::Keyboard::LControl),
	std::pair<std::string, sf::Keyboard::Key>("LSHIFT", sf::Keyboard::LShift),
	std::pair<std::string, sf::Keyboard::Key>("LALT", sf::Keyboard::LSystem),
	std::pair<std::string, sf::Keyboard::Key>("LSYSTEM", sf::Keyboard::LAlt),
	std::pair<std::string, sf::Keyboard::Key>("RCONTROL", sf::Keyboard::RControl),
	std::pair<std::string, sf::Keyboard::Key>("RSHIFT", sf::Keyboard::RShift),
	std::pair<std::string, sf::Keyboard::Key>("RALT", sf::Keyboard::RAlt),
	std::pair<std::string, sf::Keyboard::Key>("RSYSTEM", sf::Keyboard::RSystem),
	std::pair<std::string, sf::Keyboard::Key>("MENU", sf::Keyboard::Menu),
	std::pair<std::string, sf::Keyboard::Key>("LBRACKET", sf::Keyboard::LBracket),
	std::pair<std::string, sf::Keyboard::Key>("RBRACKET", sf::Keyboard::RBracket),
	std::pair<std::string, sf::Keyboard::Key>("SEMICOLON", sf::Keyboard::SemiColon),
	std::pair<std::string, sf::Keyboard::Key>("COMMA", sf::Keyboard::Comma),
	std::pair<std::string, sf::Keyboard::Key>("PERIOD", sf::Keyboard::Period),
	std::pair<std::string, sf::Keyboard::Key>("QUOTE", sf::Keyboard::Quote),
	std::pair<std::string, sf::Keyboard::Key>("SLASH", sf::Keyboard::Slash),
	std::pair<std::string, sf::Keyboard::Key>("BACKSLASH", sf::Keyboard::BackSlash),
	std::pair<std::string, sf::Keyboard::Key>("TILDE", sf::Keyboard::Tilde),
	std::pair<std::string, sf::Keyboard::Key>("EQUAL", sf::Keyboard::Equal),
	std::pair<std::string, sf::Keyboard::Key>("DASH", sf::Keyboard::Dash),
	std::pair<std::string, sf::Keyboard::Key>("SPACE", sf::Keyboard::Space),
	std::pair<std::string, sf::Keyboard::Key>("RETURN", sf::Keyboard::Return),
	std::pair<std::string, sf::Keyboard::Key>("BACKSPACE", sf::Keyboard::BackSpace),
	std::pair<std::string, sf::Keyboard::Key>("TAB", sf::Keyboard::Tab),
	std::pair<std::string, sf::Keyboard::Key>("PAGEUP", sf::Keyboard::PageUp),
	std::pair<std::string, sf::Keyboard::Key>("PAGEDOWN", sf::Keyboard::PageDown),
	std::pair<std::string, sf::Keyboard::Key>("END", sf::Keyboard::End),
	std::pair<std::string, sf::Keyboard::Key>("HOME", sf::Keyboard::Home),
	std::pair<std::string, sf::Keyboard::Key>("INSERT", sf::Keyboard::Insert),
	std::pair<std::string, sf::Keyboard::Key>("DELETE", sf::Keyboard::Delete),
	std::pair<std::string, sf::Keyboard::Key>("ADD", sf::Keyboard::Add),
	std::pair<std::string, sf::Keyboard::Key>("SUBTRACT", sf::Keyboard::Subtract),
	std::pair<std::string, sf::Keyboard::Key>("MULTIPLY", sf::Keyboard::Multiply),
	std::pair<std::string, sf::Keyboard::Key>("DIVIDE", sf::Keyboard::Divide),
	std::pair<std::string, sf::Keyboard::Key>("LEFT", sf::Keyboard::Left),
	std::pair<std::string, sf::Keyboard::Key>("RIGHT", sf::Keyboard::Right),
	std::pair<std::string, sf::Keyboard::Key>("UP", sf::Keyboard::Up),
	std::pair<std::string, sf::Keyboard::Key>("DOWN", sf::Keyboard::Down),
	std::pair<std::string, sf::Keyboard::Key>("NUMPAD0", sf::Keyboard::Numpad0),
	std::pair<std::string, sf::Keyboard::Key>("NUMPAD1", sf::Keyboard::Numpad1),
	std::pair<std::string, sf::Keyboard::Key>("NUMPAD2", sf::Keyboard::Numpad2),
	std::pair<std::string, sf::Keyboard::Key>("NUMPAD3", sf::Keyboard::Numpad3),
	std::pair<std::string, sf::Keyboard::Key>("NUMPAD4", sf::Keyboard::Numpad4),
	std::pair<std::string, sf::Keyboard::Key>("NUMPAD5", sf::Keyboard::Numpad5),
	std::pair<std::string, sf::Keyboard::Key>("NUMPAD6", sf::Keyboard::Numpad6),
	std::pair<std::string, sf::Keyboard::Key>("NUMPAD7", sf::Keyboard::Numpad7),
	std::pair<std::string, sf::Keyboard::Key>("NUMPAD8", sf::Keyboard::Numpad8),
	std::pair<std::string, sf::Keyboard::Key>("NUMPAD9", sf::Keyboard::Numpad9),
	std::pair<std::string, sf::Keyboard::Key>("F1", sf::Keyboard::F1),
	std::pair<std::string, sf::Keyboard::Key>("F2", sf::Keyboard::F2),
	std::pair<std::string, sf::Keyboard::Key>("F3", sf::Keyboard::F3),
	std::pair<std::string, sf::Keyboard::Key>("F4", sf::Keyboard::F4),
	std::pair<std::string, sf::Keyboard::Key>("F5", sf::Keyboard::F5),
	std::pair<std::string, sf::Keyboard::Key>("F6", sf::Keyboard::F6),
	std::pair<std::string, sf::Keyboard::Key>("F7", sf::Keyboard::F7),
	std::pair<std::string, sf::Keyboard::Key>("F8", sf::Keyboard::F8),
	std::pair<std::string, sf::Keyboard::Key>("F9", sf::Keyboard::F9),
	std::pair<std::string, sf::Keyboard::Key>("F10", sf::Keyboard::F10),
	std::pair<std::string, sf::Keyboard::Key>("F11", sf::Keyboard::F11),
	std::pair<std::string, sf::Keyboard::Key>("F12", sf::Keyboard::F12),
	std::pair<std::string, sf::Keyboard::Key>("F13", sf::Keyboard::F13),
	std::pair<std::string, sf::Keyboard::Key>("F14", sf::Keyboard::F14),
	std::pair<std::string, sf::Keyboard::Key>("F15", sf::Keyboard::F15),
	std::pair<std::string, sf::Keyboard::Key>("PAUSE", sf::Keyboard::Pause)
};
const std::pair<std::string, int> Controls::joystickButtons[sf::Joystick::ButtonCount] = {
	std::pair<std::string, int>("0", 0),
	std::pair<std::string, int>("1", 1),
	std::pair<std::string, int>("2", 2),
	std::pair<std::string, int>("3", 3),
	std::pair<std::string, int>("4", 4),
	std::pair<std::string, int>("5", 5),
	std::pair<std::string, int>("6", 6),
	std::pair<std::string, int>("7", 7),
	std::pair<std::string, int>("8", 8),
	std::pair<std::string, int>("9", 9),
	std::pair<std::string, int>("10", 10),
	std::pair<std::string, int>("11", 11),
	std::pair<std::string, int>("12", 12),
	std::pair<std::string, int>("13", 13),
	std::pair<std::string, int>("14", 14),
	std::pair<std::string, int>("15", 15),
	std::pair<std::string, int>("16", 16),
	std::pair<std::string, int>("17", 17),
	std::pair<std::string, int>("18", 18),
	std::pair<std::string, int>("19", 19),
	std::pair<std::string, int>("20", 20),
	std::pair<std::string, int>("21", 21),
	std::pair<std::string, int>("22", 22),
	std::pair<std::string, int>("23", 23),
	std::pair<std::string, int>("24", 24),
	std::pair<std::string, int>("25", 25),
	std::pair<std::string, int>("26", 26),
	std::pair<std::string, int>("27", 27),
	std::pair<std::string, int>("28", 28),
	std::pair<std::string, int>("29", 29),
	std::pair<std::string, int>("30", 30),
	std::pair<std::string, int>("31", 31)
};
const std::pair<std::string, sf::Joystick::Axis> Controls::joystickAxis[sf::Joystick::AxisCount] = {
	std::pair<std::string, sf::Joystick::Axis>("X", sf::Joystick::Axis::X),
	std::pair<std::string, sf::Joystick::Axis>("Y", sf::Joystick::Axis::Y),
	std::pair<std::string, sf::Joystick::Axis>("Z", sf::Joystick::Axis::Z),
	std::pair<std::string, sf::Joystick::Axis>("R", sf::Joystick::Axis::R),
	std::pair<std::string, sf::Joystick::Axis>("U", sf::Joystick::Axis::U),
	std::pair<std::string, sf::Joystick::Axis>("V", sf::Joystick::Axis::V),
	std::pair<std::string, sf::Joystick::Axis>("POVX", sf::Joystick::Axis::PovX),
	std::pair<std::string, sf::Joystick::Axis>("POVY", sf::Joystick::Axis::PovY)
};
