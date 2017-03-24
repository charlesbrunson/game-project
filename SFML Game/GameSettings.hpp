#ifndef GAMESETTINGS_H
#define GAMESETTINGS_H

#include <SFML/System.hpp>
#include <vector>

enum SettingsField : int {
	GameVolume = 0,
	MusicVolume = 1,
	WINDOWED_VSYNC,
	FULLSCREEN_VSYNC,
	WINDOWMARGIN,
	CONTROLS_UP,
	CONTROLS_DOWN,
	CONTROLS_LEFT,
	CONTROLS_RIGHT,
	CONTROLS_JUMP,
	CONTROLS_ATTACK,
	CONTROLS_DASH,
	CONTROLS_SELECT,
	CONTROLS_START,
	JOYSTICK_UP,
	JOYSTICK_DOWN,
	JOYSTICK_LEFT,
	JOYSTICK_RIGHT,
	JOYSTICK_JUMP,
	JOYSTICK_ATTACK,
	JOYSTICK_DASH,
	JOYSTICK_SELECT,
	JOYSTICK_START,
	COUNT
};

class SettingsListener {
	virtual void notifyChangedSetting(SettingsField field, sf::String value) = 0;
};

class GameSettings {
public:

	static sf::String getSettingValue(SettingsField field);
	static void setSettingValue(SettingsField field, sf::String value);

	static void readSettingsFile();
	static void writeSettingsFile();

	static void addListener(SettingsListener* listener) {
		listeners.push_back(listener);
	};
	static bool removeListener(SettingsListener* listener) {
		for (auto i = listeners.begin(); i != listeners.end(); i++) {
			if ((SettingsListener*)(i._Ptr) == listener) {
				listeners.erase(i);
				return true;
			}
		}
		return false;
	};

private:
	struct Setting {
		Setting(std::string setting, std::string value, std::string comment, bool addLine = true) {
			s = setting;
			v = value;
			c = comment;
			emptyLine = addLine;
		}

		std::string s;
		std::string v;
		std::string c;
		bool emptyLine;
	};

	static void notifyAll(SettingsField field, sf::String value);

	//location of settings file relative to application
	const static std::string SETTINGSPATH;

	//the list of game settings and their defaults
	static Setting settings[SettingsField::COUNT];

	static std::vector<SettingsListener*> listeners;
};

#endif