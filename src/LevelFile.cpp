#include "res/LevelFile.hpp"

#include <sstream>

#include "res/ResourceLoader.hpp"
#include "util/Log.hpp"
#include "util/StandardSize.hpp"
#include "Globals.hpp"

#include "game/lvl/LevelLoader.hpp"

#if defined(_DEBUG) && defined(_WIN32)
#include <Windows.h>
#elif defined(_DEBUG) && defined(__linux__)
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

LevelFile::LevelFile(std::string path, FileStream* str) : GameFile(path, str) {
	fileType = FileType::LEVEL;
	load(path, str);
};


bool LevelFile::openData(std::ifstream* str) {
	if (str->is_open())
		str->close();

	if (packPos != -1) {
		// find file in pack
		str->open(RL()->packName, std::ios::binary);
		if (str->is_open()) {
			str->seekg(RL()->getPackHeaderSize() + packPos);
			return true;
		}
	}
	else {
		//find file in data directory
		str->open(RL()->fileDir + filePath, std::ios::binary);
		return str->is_open();
	}
	return false;
}

bool LevelFile::loadFromFile(std::string path) {

	// recompile .lvl file from .tmx
	std::string lvlName = RL()->fileDir + path;
	std::string tmxName = RL()->fileDir + path;
	tmxName.erase(lvlName.find_first_of(".lvl"), lvlName.length());
	tmxName.append(".tmx");

	bool toCompile = false;

#if defined(_DEBUG) && defined(_WIN32)
	// Auto recompile .lvl file if older than the associated .tmx file

	HANDLE lvlFileHandle = CreateFile(lvlName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE tmxFileHandle = CreateFile(tmxName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	FILETIME tmxWriteTime, lvlWriteTime;

	bool lvlExists = GetFileTime(lvlFileHandle, NULL, NULL, &lvlWriteTime) != 0;
	bool tmxExists = GetFileTime(tmxFileHandle, NULL, NULL, &tmxWriteTime) != 0;

	CloseHandle(lvlFileHandle);
	CloseHandle(tmxFileHandle);

	// lvl is older than tmx file, needs recompile
	toCompile = tmxExists && (!lvlExists || (lvlExists && CompareFileTime(&lvlWriteTime, &tmxWriteTime) < 0));

#elif defined(_DEBUG) && defined(__linux__)

	struct stat lvlFileHandle;
	struct stat tmxFileHandle;

	bool lvlExists = stat(lvlName.c_str(), &lvlFileHandle) == 0;
	bool tmxExists = stat(tmxName.c_str(), &tmxFileHandle) == 0;

	// lvl is older than tmx file, needs recompile
	toCompile = tmxExists && (!lvlExists || (lvlExists && lvlFileHandle.st_mtime < tmxFileHandle.st_mtime));

#endif

	// If the .lvl file needs to be recompiled, do so
	if (toCompile || Gameplay_Globals::Debug::forceCompile) {

		// Get full file path from directory
		std::string tmxFile = tmxName;
		if (Gameplay_Globals::Debug::forceCompile) {
			Log::msg("Recompiling " + tmxFile + " --- Forced");
		}
		else {
			Log::msg("Recompiling " + tmxFile);
		}
		tmxFile.erase(tmxFile.length() - 4, tmxFile.length());
		tmxFile.erase(0, RL()->fileDir.length());

		// Pass to level tmx compiler
		LevelLoader::compileTMXFile(tmxFile);
	}

	//store as data
	clearData();

	std::ifstream reader(RL()->fileDir + path, std::ios_base::binary);
	if (reader.is_open()) {

		std::stringstream buffer;
		buffer << reader.rdbuf();

		data.reserve(reader.tellg());
		data = buffer.str();
		data.shrink_to_fit();

		reader.close();
		return true;
	}
	return false;
}

bool LevelFile::loadFromStream(FileStream* str) {
	
	packPos = str->getSource()->tellg();
	return true;
}

void LevelFile::convertToData() {

}
