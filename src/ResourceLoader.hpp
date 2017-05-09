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

// Singleton
// Loads and stores resources for program
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
	const sf::Texture& getTexture(std::string filename);
	bool isTextureTileset(std::string filename);

	// Sound buffer retrieval
	const sf::SoundBuffer& getSoundBuffer(std::string filename);

	// Font retrieval
	const sf::Font& getFont(std::string filename);

	// Shader retrieval
	sf::Shader* getShader(std::string filename);

	// Determines optimal texture size, in powers of 2 (ie, 16x16, 32x32, etc)
	static sf::Vector2u getIdealTexSize(sf::Vector2u size);
	
	// Retrieves position of level data in pack file
	int getLevelOffset(std::string levelName) {
		auto i = levels.find(levelName);
		if (i != levels.end())
			return i->second;

		return -1;
	}

	// Resource directory and pack file constants
	const static std::string fileDir;
	const static std::string fileIndex;
	const static std::string packName;
	const static int packHeaderSize;

	// Data type folder names
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

	// Name, data offset of levels in pack file
	std::map<std::string, int> levels;

	// List of level names
	//std::vector<std::string> levelPaths;
	
	// Retrieves files
	template <class T>
	T* fetchResource(std::string filename, std::map<std::string, T> &map);
};

//shorthand for ResourceLoader::get()
ResourceLoader* RL();

#endif