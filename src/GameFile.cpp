#include "GameFile.hpp"

#include "Log.hpp"

#include "TextureFile.hpp"

GameFile::GameFile(std::string path, FileStream* str) {
	filePath = path;
	load(path, str);
};

bool GameFile::load(std::string path, FileStream* str) {
	bool r = str ? in_loadFromStream(str) : in_loadFromFile(path);

	if (r && !str)
		convertToData();

	return r;
};

const char* GameFile::getData() {
	if (dataSize == 0)
		convertToData();

	return data;
}

const int GameFile::getDataSize() {
	if (dataSize == 0)
		convertToData();

	return dataSize;
}

// factory methods for creating appropriate GameFile for file's extension type
GameFile* createFile(std::string path, FileStream* str) {

	Log::msg("\nCreating file for " + path + "\n");

	std::string extension = path.substr(path.find_last_of('.'), path.size());

	Log::msg("Extension detected: " + extension + "\n");

	Log::msg("Creating GameFile type: ");

	// image file
	if (extension == ".png") {
		Log::msg(".png\n");
		return new TextureFile(path, str);
	}
	// font
	else if (extension == ".tff") {
		Log::msg(".tff\n");
		//TODO
	}
	// level
	else if (extension == ".lvl") {
		Log::msg(".lvl\n");
		//TODO
	}
	// fragment shader
	else if (extension == ".frag") {
		Log::msg(".frag\n");
		//TODO
	}
	// vertex shader
	else if (extension == ".vert") {
		Log::msg(".vert\n");
		//TODO
	}


}