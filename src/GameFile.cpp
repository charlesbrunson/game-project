#include "GameFile.hpp"

#include "Log.hpp"

#include "GenericFile.hpp"
#include "TextureFile.hpp"
#include "FontFile.hpp"

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

	Log::msg("\nCreating file for " + path + "\n");

	std::string extension = path.substr(path.find_last_of('.'), path.size());

	Log::msg("Extension detected: " + extension + "\n");

	Log::msg("Creating GameFile type: ");

	// image file
	if (extension == ".png") {
		Log::msg(".png");
		return new TextureFile(path, str);
	}
	// font
	else if (extension == ".ttf") {
		Log::msg(".ttf");
		return new FontFile(path, str);
	}
	// level
	else if (extension == ".lvl") {
		Log::msg(".lvl");
		//TODO
	}
	// fragment shader
	else if (extension == ".frag") {
		Log::msg(".frag");
		return new GenericFile(path, str);
	}
	// vertex shader
	else if (extension == ".vert") {
		Log::msg(".vert");
		return new GenericFile(path, str);
	}
	// text
	else if (extension == ".txt") {
		Log::msg(".txt");
		return new GenericFile(path, str);
	}
	Log::msg("\n");

	return nullptr;
}