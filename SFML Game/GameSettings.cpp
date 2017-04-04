#include <fstream>
#include "GameSettings.hpp"
#include "Log.hpp"


const std::string GameSettings::SETTINGSPATH = "settings.txt";

GameSettings::Setting GameSettings::settings[SettingsField::COUNT] = {
	Setting("gamevolume", "50", "Volume of gameplay sounds [0...100]"),
	Setting("musicvolume", "50", "Volume of game music [0...100]"),
	Setting("windowed_vsync", "0", "Enable Vsync for windowed mode [0, 1]"),
	Setting("fullscreen_vsync", "1", "Enable Vsync for fullscreen [0, 1]"),
	Setting("allow_window_margin", "1", "Windowed behavior when resized [1, 0]"),
	Setting("KB_Up", "W", "Player Controls - Keyboard", false),
	Setting("KB_Down", "S", "", false),
	Setting("KB_Left", "A", "", false),
	Setting("KB_Right", "D", "", false),
	Setting("KB_Jump", "Space", "", false),
	Setting("KB_Attack", "K", "", false),
	Setting("KB_Dash", "LShift", "", false),
	Setting("KB_Select", "RShift", "", false),
	Setting("KB_Start", "Return", "", true),
	Setting("JS_Up", ">POVY", "Player Controls - Joystick/Controller", false),
	Setting("JS_Down", "<POVY", "", false),
	Setting("JS_Left", "<POVX", "", false),
	Setting("JS_Right", ">POVX", "", false),
	Setting("JS_Jump", "0", "", false),
	Setting("JS_Attack", "2", "", false),
	Setting("JS_Dash", "1", "", false),
	Setting("JS_Select", "6", "", false),
	Setting("JS_Start", "7", "", false)
};

std::string GameSettings::getSettingValue(SettingsField field) {
	return settings[field].v;
}

void GameSettings::setSettingValue(SettingsField field, std::string value) {
	settings[field].v = value;
}

void GameSettings::readSettingsFile() {

	std::ifstream reader(SETTINGSPATH);

	if (!reader.is_open())
		return;

	Log::msg("Reading Settings...\n");

	std::string line;
	while (std::getline(reader, line)) {

		//ignore comment lines
		if (line.size() == 0 || line.at(0) == '#')
			continue;

		//clear whitespace
		line.erase(remove_if(line.begin(), line.end(), isspace), line.end());

		//parse line
		int p = line.find_first_of('=');

		if (p == line.length() || p == std::string::npos)
			continue;

		std::string setting = line.substr(0, p);
		std::string value = line.substr(p + 1, line.length());

		Log::msg("\t\"" + setting + "\" = \"" + value + "\"\n");

		//find setting in array
		for (int i = 0; i < SettingsField::COUNT; i++) {
			if (settings[i].s == std::string(setting)) {
				settings[i].v = value;
				break;
			}
		}
	}

	reader.close();
}

void GameSettings::writeSettingsFile() {
	std::ofstream writer(SETTINGSPATH);

	if (!writer.is_open())
		return;

	Log::msg("Writing Settings...\n");

	for (int i = 0; i < SettingsField::COUNT; i++) {

		if (!settings[i].c.empty())
			writer << "#" << settings[i].c << "\n";

		writer << settings[i].s << "=" << settings[i].v << "\n" << (settings[i].emptyLine? "\n" : "");

	}

	writer.close();
}