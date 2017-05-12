#ifndef RESOURCELOADER_H
#define RESOURCELOADER_H

#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include <map>
#include <mutex>
#include <fstream>
#include <memory>

#include "GenericFile.hpp"
#include "TextureFile.hpp"
#include "FontFile.hpp"
#include "LevelFile.hpp"

// Singleton
// Loads and stores assets for program
class ResourceLoader {

	static ResourceLoader resourceLoader;

	ResourceLoader() = default;
	~ResourceLoader() = default;

public:

	static ResourceLoader* get() {
		return &resourceLoader;
	}

	// Allocating and Deallocating resources
	bool loadResources();
	void dumpResources();

	// Texture retrieval
	const TextureFile& getTexFile(std::string filename);
	const sf::Texture& getTexture(std::string filename);
	const std::string& getGeneric(std::string filename);
	const sf::Font& getFont(std::string filename);
	sf::Shader* getShader(std::string filename);

	int getLevelOffset(std::string lvlname);

	// Sound buffer retrieval
	// const sf::SoundBuffer& getSoundBuffer(std::string filename);

	// Font retrieval
	// const sf::Font& getFont(std::string filename);

	// Shader retrieval
	// sf::Shader* getShader(std::string filename);

	// Resource directory and pack file constants
	const static std::string fileDir;
	const static std::string fileIndex;
	const static std::string packName;
	int packHeaderSize;

	// Data type folder names
	/*
	enum fileTypeEnum : int {
	SPRITE = 0,
	TILESET,
	SOUND,
	FONT,
	LEVEL,
	SHADERS,
	TYPE_COUNT
	};
	static const std::string fileTypes[TYPE_COUNT];
	*/

	bool isLoaded() {
		return loaded;
	}

private:
	bool loaded = false;

	// Reads data directly from directory file
	void loadFromFile();
	// Writes data to resource pack file
	void writeToPack();
	// Read data from the resource pack file
	bool loadFromPack();

	// Mutex to prevent multiple simultaneous access of resources
	std::mutex m;

	// Maps containing resources
	//TODO
	std::map<std::string, TextureFile*> textures;
	std::map<std::string, GenericFile*> generics;
	std::map<std::string, FontFile*> fonts;
	std::map<std::string, LevelFile*> levels;


	// Name, data offset of levels in pack file
	//std::map<std::string, int> levels;

	// List of level names
	//std::vector<std::string> levelPaths;

};

//shorthand for ResourceLoader::get()
ResourceLoader* RL();

#endif
