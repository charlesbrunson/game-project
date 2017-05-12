#include "GameFile.hpp"

#include "Log.hpp"

#include "GenericFile.hpp"
#include "TextureFile.hpp"
#include "FontFile.hpp"
#include "LevelFile.hpp"

GameFile::GameFile(std::string path, FileStream* str) {
	filePath = path;
};

bool GameFile::load(std::string path, FileStream* str) {
	bool r = false;
	if (str) {
		r = loadFromStream(str);
	}
	else {
		r = loadFromFile(path);
	}

	if (r && !str) {
		convertToData();
	}

	//validData = !data.empty();
	return r;
};

const std::string* GameFile::getData() {
	if (data.empty())
		convertToData();

	//validData = !data.empty();
	return &data;
}

const int GameFile::getDataSize() {
	if (data.empty())
		convertToData();

	//validData = !data.empty();
	return data.size();
}
void GameFile::clearData() {
	data.clear();
	//validData = false;
}

// factory methods for creating appropriate GameFile for file's extension type
GameFile* GameFile::create(std::string path, FileStream* str) {

	Log::msg("\nCreating file for " + path);

	std::string extension = path.substr(path.find_last_of('.'), path.size());
	
	// image file
	if (extension == ".png") {
		return new TextureFile(path, str);
	}
	// font
	else if (extension == ".ttf") {
		return new FontFile(path, str);
	}
	// level
	else if (extension == ".lvl") {
		return new LevelFile(path, str);
	}
	// fragment shader
	else if (extension == ".frag") {
		return new GenericFile(path, str);
	}
	// vertex shader
	else if (extension == ".vert") {
		return new GenericFile(path, str);
	}
	// text
	else if (extension == ".txt") {
		return new GenericFile(path, str);
	}

	return nullptr;
}