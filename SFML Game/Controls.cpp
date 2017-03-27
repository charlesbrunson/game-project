#include "Controls.hpp"

//these are the defaults, could be overwritten by something
sf::Keyboard::Key Controls::FullscreenToggle = sf::Keyboard::F9;

//player controls
sf::Keyboard::Key Controls::UpK = sf::Keyboard::W;
sf::Keyboard::Key Controls::LeftK = sf::Keyboard::A;
sf::Keyboard::Key Controls::DownK = sf::Keyboard::S;
sf::Keyboard::Key Controls::RightK = sf::Keyboard::D;

sf::Keyboard::Key Controls::JumpK = sf::Keyboard::Space;
sf::Keyboard::Key Controls::AttackK = sf::Keyboard::K;
sf::Keyboard::Key Controls::SprintK = sf::Keyboard::LShift;

sf::Keyboard::Key Controls::StartK = sf::Keyboard::Return;
sf::Keyboard::Key Controls::SelectK = sf::Keyboard::RShift;

sf::Keyboard::Key* const Controls::keyboardKeys[Controls::Input::INPUT_COUNT] = {
	&Controls::UpK,
	&Controls::LeftK,
	&Controls::DownK,
	&Controls::RightK,
	&Controls::JumpK,
	&Controls::AttackK,
	&Controls::SprintK,
	&Controls::StartK,
	&Controls::SelectK
};

Controls::JoystickInput Controls::UpJ;
Controls::JoystickInput Controls::LeftJ;
Controls::JoystickInput Controls::DownJ;
Controls::JoystickInput Controls::RightJ;

Controls::JoystickInput Controls::JumpJ;
Controls::JoystickInput Controls::AttackJ;
Controls::JoystickInput Controls::SprintJ;

Controls::JoystickInput Controls::StartJ;
Controls::JoystickInput Controls::SelectJ;

Controls::JoystickInput* const Controls::joystickKeys[Controls::Input::INPUT_COUNT] = {
	&Controls::UpJ,
	&Controls::LeftJ,
	&Controls::DownJ,
	&Controls::RightJ,
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
	&Controls::LeftActive,
	&Controls::DownActive,
	&Controls::RightActive,
	&Controls::JumpActive,
	&Controls::AttackActive,
	&Controls::SprintActive,
	&Controls::StartActive,
	&Controls::SelectActive
};


// Mouse input
sf::Vector2f Controls::mousePosition;
bool Controls::mouseInWindow;
Controls::MouseInput Controls::mouseActive;
sf::Time Controls::mouseLastMoved = sf::Time::Zero;


const std::pair<sf::String, sf::Keyboard::Key> Controls::keyboardKeyStrings[sf::Keyboard::KeyCount] =
{
	std::pair<sf::String, sf::Keyboard::Key>("A", sf::Keyboard::A),
	std::pair<sf::String, sf::Keyboard::Key>("B", sf::Keyboard::B),
	std::pair<sf::String, sf::Keyboard::Key>("C", sf::Keyboard::C),
	std::pair<sf::String, sf::Keyboard::Key>("D", sf::Keyboard::D),
	std::pair<sf::String, sf::Keyboard::Key>("E", sf::Keyboard::E),
	std::pair<sf::String, sf::Keyboard::Key>("F", sf::Keyboard::F),
	std::pair<sf::String, sf::Keyboard::Key>("G", sf::Keyboard::G),
	std::pair<sf::String, sf::Keyboard::Key>("H", sf::Keyboard::H),
	std::pair<sf::String, sf::Keyboard::Key>("I", sf::Keyboard::I),
	std::pair<sf::String, sf::Keyboard::Key>("J", sf::Keyboard::J),
	std::pair<sf::String, sf::Keyboard::Key>("K", sf::Keyboard::K),
	std::pair<sf::String, sf::Keyboard::Key>("L", sf::Keyboard::L),
	std::pair<sf::String, sf::Keyboard::Key>("M", sf::Keyboard::M),
	std::pair<sf::String, sf::Keyboard::Key>("N", sf::Keyboard::N),
	std::pair<sf::String, sf::Keyboard::Key>("O", sf::Keyboard::O),
	std::pair<sf::String, sf::Keyboard::Key>("P", sf::Keyboard::P),
	std::pair<sf::String, sf::Keyboard::Key>("Q", sf::Keyboard::Q),
	std::pair<sf::String, sf::Keyboard::Key>("R", sf::Keyboard::R),
	std::pair<sf::String, sf::Keyboard::Key>("S", sf::Keyboard::S),
	std::pair<sf::String, sf::Keyboard::Key>("T", sf::Keyboard::T),
	std::pair<sf::String, sf::Keyboard::Key>("U", sf::Keyboard::U),
	std::pair<sf::String, sf::Keyboard::Key>("V", sf::Keyboard::V),
	std::pair<sf::String, sf::Keyboard::Key>("W", sf::Keyboard::W),
	std::pair<sf::String, sf::Keyboard::Key>("X", sf::Keyboard::X),
	std::pair<sf::String, sf::Keyboard::Key>("Y", sf::Keyboard::Y),
	std::pair<sf::String, sf::Keyboard::Key>("Z", sf::Keyboard::Z),
	std::pair<sf::String, sf::Keyboard::Key>("NUM0", sf::Keyboard::Num0),
	std::pair<sf::String, sf::Keyboard::Key>("NUM1", sf::Keyboard::Num1),
	std::pair<sf::String, sf::Keyboard::Key>("NUM2", sf::Keyboard::Num2),
	std::pair<sf::String, sf::Keyboard::Key>("NUM3", sf::Keyboard::Num3),
	std::pair<sf::String, sf::Keyboard::Key>("NUM4", sf::Keyboard::Num4),
	std::pair<sf::String, sf::Keyboard::Key>("NUM5", sf::Keyboard::Num5),
	std::pair<sf::String, sf::Keyboard::Key>("NUM6", sf::Keyboard::Num6),
	std::pair<sf::String, sf::Keyboard::Key>("NUM7", sf::Keyboard::Num7),
	std::pair<sf::String, sf::Keyboard::Key>("NUM8", sf::Keyboard::Num8),
	std::pair<sf::String, sf::Keyboard::Key>("NUM9", sf::Keyboard::Num9),
	std::pair<sf::String, sf::Keyboard::Key>("ESCAPE", sf::Keyboard::Escape),
	std::pair<sf::String, sf::Keyboard::Key>("LCONTROL", sf::Keyboard::LControl),
	std::pair<sf::String, sf::Keyboard::Key>("LSHIFT", sf::Keyboard::LShift),
	std::pair<sf::String, sf::Keyboard::Key>("LALT", sf::Keyboard::LSystem),
	std::pair<sf::String, sf::Keyboard::Key>("LSYSTEM", sf::Keyboard::LAlt),
	std::pair<sf::String, sf::Keyboard::Key>("RCONTROL", sf::Keyboard::RControl),
	std::pair<sf::String, sf::Keyboard::Key>("RSHIFT", sf::Keyboard::RShift),
	std::pair<sf::String, sf::Keyboard::Key>("RALT", sf::Keyboard::RAlt),
	std::pair<sf::String, sf::Keyboard::Key>("RSYSTEM", sf::Keyboard::RSystem),
	std::pair<sf::String, sf::Keyboard::Key>("MENU", sf::Keyboard::Menu),
	std::pair<sf::String, sf::Keyboard::Key>("LBRACKET", sf::Keyboard::LBracket),
	std::pair<sf::String, sf::Keyboard::Key>("RBRACKET", sf::Keyboard::RBracket),
	std::pair<sf::String, sf::Keyboard::Key>("SEMICOLON", sf::Keyboard::SemiColon),
	std::pair<sf::String, sf::Keyboard::Key>("COMMA", sf::Keyboard::Comma),
	std::pair<sf::String, sf::Keyboard::Key>("PERIOD", sf::Keyboard::Period),
	std::pair<sf::String, sf::Keyboard::Key>("QUOTE", sf::Keyboard::Quote),
	std::pair<sf::String, sf::Keyboard::Key>("SLASH", sf::Keyboard::Slash),
	std::pair<sf::String, sf::Keyboard::Key>("BACKSLASH", sf::Keyboard::BackSlash),
	std::pair<sf::String, sf::Keyboard::Key>("TILDE", sf::Keyboard::Tilde),
	std::pair<sf::String, sf::Keyboard::Key>("EQUAL", sf::Keyboard::Equal),
	std::pair<sf::String, sf::Keyboard::Key>("DASH", sf::Keyboard::Dash),
	std::pair<sf::String, sf::Keyboard::Key>("SPACE", sf::Keyboard::Space),
	std::pair<sf::String, sf::Keyboard::Key>("RETURN", sf::Keyboard::Return),
	std::pair<sf::String, sf::Keyboard::Key>("BACKSPACE", sf::Keyboard::BackSpace),
	std::pair<sf::String, sf::Keyboard::Key>("TAB", sf::Keyboard::Tab),
	std::pair<sf::String, sf::Keyboard::Key>("PAGEUP", sf::Keyboard::PageUp),
	std::pair<sf::String, sf::Keyboard::Key>("PAGEDOWN", sf::Keyboard::PageDown),
	std::pair<sf::String, sf::Keyboard::Key>("END", sf::Keyboard::End),
	std::pair<sf::String, sf::Keyboard::Key>("HOME", sf::Keyboard::Home),
	std::pair<sf::String, sf::Keyboard::Key>("INSERT", sf::Keyboard::Insert),
	std::pair<sf::String, sf::Keyboard::Key>("DELETE", sf::Keyboard::Delete),
	std::pair<sf::String, sf::Keyboard::Key>("ADD", sf::Keyboard::Add),
	std::pair<sf::String, sf::Keyboard::Key>("SUBTRACT", sf::Keyboard::Subtract),
	std::pair<sf::String, sf::Keyboard::Key>("MULTIPLY", sf::Keyboard::Multiply),
	std::pair<sf::String, sf::Keyboard::Key>("DIVIDE", sf::Keyboard::Divide),
	std::pair<sf::String, sf::Keyboard::Key>("LEFT", sf::Keyboard::Left),
	std::pair<sf::String, sf::Keyboard::Key>("RIGHT", sf::Keyboard::Right),
	std::pair<sf::String, sf::Keyboard::Key>("UP", sf::Keyboard::Up),
	std::pair<sf::String, sf::Keyboard::Key>("DOWN", sf::Keyboard::Down),
	std::pair<sf::String, sf::Keyboard::Key>("NUMPAD0", sf::Keyboard::Numpad0),
	std::pair<sf::String, sf::Keyboard::Key>("NUMPAD1", sf::Keyboard::Numpad1),
	std::pair<sf::String, sf::Keyboard::Key>("NUMPAD2", sf::Keyboard::Numpad2),
	std::pair<sf::String, sf::Keyboard::Key>("NUMPAD3", sf::Keyboard::Numpad3),
	std::pair<sf::String, sf::Keyboard::Key>("NUMPAD4", sf::Keyboard::Numpad4),
	std::pair<sf::String, sf::Keyboard::Key>("NUMPAD5", sf::Keyboard::Numpad5),
	std::pair<sf::String, sf::Keyboard::Key>("NUMPAD6", sf::Keyboard::Numpad6),
	std::pair<sf::String, sf::Keyboard::Key>("NUMPAD7", sf::Keyboard::Numpad7),
	std::pair<sf::String, sf::Keyboard::Key>("NUMPAD8", sf::Keyboard::Numpad8),
	std::pair<sf::String, sf::Keyboard::Key>("NUMPAD9", sf::Keyboard::Numpad9),
	std::pair<sf::String, sf::Keyboard::Key>("F1", sf::Keyboard::F1),
	std::pair<sf::String, sf::Keyboard::Key>("F2", sf::Keyboard::F2),
	std::pair<sf::String, sf::Keyboard::Key>("F3", sf::Keyboard::F3),
	std::pair<sf::String, sf::Keyboard::Key>("F4", sf::Keyboard::F4),
	std::pair<sf::String, sf::Keyboard::Key>("F5", sf::Keyboard::F5),
	std::pair<sf::String, sf::Keyboard::Key>("F6", sf::Keyboard::F6),
	std::pair<sf::String, sf::Keyboard::Key>("F7", sf::Keyboard::F7),
	std::pair<sf::String, sf::Keyboard::Key>("F8", sf::Keyboard::F8),
	std::pair<sf::String, sf::Keyboard::Key>("F9", sf::Keyboard::F9),
	std::pair<sf::String, sf::Keyboard::Key>("F10", sf::Keyboard::F10),
	std::pair<sf::String, sf::Keyboard::Key>("F11", sf::Keyboard::F11),
	std::pair<sf::String, sf::Keyboard::Key>("F12", sf::Keyboard::F12),
	std::pair<sf::String, sf::Keyboard::Key>("F13", sf::Keyboard::F13),
	std::pair<sf::String, sf::Keyboard::Key>("F14", sf::Keyboard::F14),
	std::pair<sf::String, sf::Keyboard::Key>("F15", sf::Keyboard::F15),
	std::pair<sf::String, sf::Keyboard::Key>("PAUSE", sf::Keyboard::Pause)
};
const std::pair<sf::String, int> Controls::joystickButtons[sf::Joystick::ButtonCount] = {
	std::pair<sf::String, int>("0", 0),
	std::pair<sf::String, int>("1", 1),
	std::pair<sf::String, int>("2", 2),
	std::pair<sf::String, int>("3", 3),
	std::pair<sf::String, int>("4", 4),
	std::pair<sf::String, int>("5", 5),
	std::pair<sf::String, int>("6", 6),
	std::pair<sf::String, int>("7", 7),
	std::pair<sf::String, int>("8", 8),
	std::pair<sf::String, int>("9", 9),
	std::pair<sf::String, int>("10", 10),
	std::pair<sf::String, int>("11", 11),
	std::pair<sf::String, int>("12", 12),
	std::pair<sf::String, int>("13", 13),
	std::pair<sf::String, int>("14", 14),
	std::pair<sf::String, int>("15", 15),
	std::pair<sf::String, int>("16", 16),
	std::pair<sf::String, int>("17", 17),
	std::pair<sf::String, int>("18", 18),
	std::pair<sf::String, int>("19", 19),
	std::pair<sf::String, int>("20", 20),
	std::pair<sf::String, int>("21", 21),
	std::pair<sf::String, int>("22", 22),
	std::pair<sf::String, int>("23", 23),
	std::pair<sf::String, int>("24", 24),
	std::pair<sf::String, int>("25", 25),
	std::pair<sf::String, int>("26", 26),
	std::pair<sf::String, int>("27", 27),
	std::pair<sf::String, int>("28", 28),
	std::pair<sf::String, int>("29", 29),
	std::pair<sf::String, int>("30", 30),
	std::pair<sf::String, int>("31", 31)
};
const std::pair<sf::String, sf::Joystick::Axis> Controls::joystickAxis[sf::Joystick::AxisCount] = {
	std::pair<sf::String, sf::Joystick::Axis>("X", sf::Joystick::Axis::X),
	std::pair<sf::String, sf::Joystick::Axis>("Y", sf::Joystick::Axis::Y),
	std::pair<sf::String, sf::Joystick::Axis>("Z", sf::Joystick::Axis::Z),
	std::pair<sf::String, sf::Joystick::Axis>("R", sf::Joystick::Axis::R),
	std::pair<sf::String, sf::Joystick::Axis>("U", sf::Joystick::Axis::U),
	std::pair<sf::String, sf::Joystick::Axis>("V", sf::Joystick::Axis::V),
	std::pair<sf::String, sf::Joystick::Axis>("POVX", sf::Joystick::Axis::PovX),
	std::pair<sf::String, sf::Joystick::Axis>("POVY", sf::Joystick::Axis::PovY)
};