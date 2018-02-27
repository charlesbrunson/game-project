#include "GameFile.hpp"

#include "Log.hpp"

#include "GenericFile.hpp"
#include "TextureFile.hpp"
#include "FontFile.hpp"
#include "LevelFile.hpp"
#include "ShaderFile.hpp"

GameFile::GameFile(std::string path, FileStream*) {
	filePath = path;
};

bool GameFile::load(std::string path, FileStream* str) {
	filePath = path;

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

	return r;
};

const std::string* GameFile::getData() {
	if (data.empty())
		convertToData();

	return &data;
}

int GameFile::getDataSize() {
	if (data.empty())
		convertToData();

	return data.size();
}
void GameFile::clearData() {
	data.clear();
}

// factory methods for creating appropriate GameFile for file's extension type
GameFile* GameFile::create(std::string path, FileStream* str) {

	Log::msg("Creating file for " + path);

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
		return new ShaderFile(sf::Shader::Type::Fragment, path, str);
	}
	// vertex shader
	else if (extension == ".vert") {
		return new ShaderFile(sf::Shader::Type::Vertex, path, str);
	}
	// text
	else if (extension == ".txt") {
		return new GenericFile(path, str);
	}

	return nullptr;
}
