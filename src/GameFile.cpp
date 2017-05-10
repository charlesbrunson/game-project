#include "GameFile.hpp"

#include "Log.hpp"

#include "GenericFile.hpp"
#include "TextureFile.hpp"

GameFile::GameFile(std::string path, FileStream* str) {
	filePath = path;
	load(path, str);
};

bool GameFile::load(std::string path, FileStream* str) {
	bool r = str ? in_loadFromStream(str) : in_loadFromFile(path);

	if (r && !str) {
		convertToData();
	}

	validData = dataSize > 0;
	return r;
};

const char* GameFile::getData() {
	if (dataSize == 0)
		convertToData();

	validData = dataSize > 0;
	return data;
}

const int GameFile::getDataSize() {
	if (dataSize == 0)
		convertToData();

	validData = dataSize > 0;
	return dataSize;
}
void GameFile::clearData() {
	delete[] data;
	validData = false;
}

// factory methods for creating appropriate GameFile for file's extension type
GameFile* create(std::string path, FileStream* str) {

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
	else if (extension == ".tff") {
		Log::msg(".tff");
		//TODO
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
	Log::msg("\n");

	return nullptr;
}