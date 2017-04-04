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

	// Constructor
	ResourceLoader() {
		//loadResources();
	};

	// Destructors, deallocates all resources
	~ResourceLoader() {
		dumpResources();
	};

public:

	static ResourceLoader* get() {
		return &resourceLoader;
	}

	// Contains textures and metadata
	struct TexData {
		bool isTileset = false;
		sf::Texture t;
	};

	// Contains fonts
	struct FontData {
		FontData(int dataSize, std::ifstream *str) : fontDataSize(dataSize) {
			fontData = new char[fontDataSize];
			str->read(fontData, fontDataSize);

			if (font.loadFromMemory(fontData, fontDataSize)) {
				valid = true;
			}
		}
		const int fontDataSize;
		char * fontData;
		sf::Font font;
		bool valid = false;
	};

	/*
	// Constructor
	ResourceLoader() {
		loadResources();
	};

	// Destructors, deallocates all resources
	~ResourceLoader() {
		dumpResources();
	};
	*/

	// Allocating and Deallocating resources
	void loadResources();
	void dumpResources();

	// Texture retrieval
	sf::Texture* getTexture(std::string filename);
	bool isTextureTileset(std::string filename);

	// Sound buffer retrieval
	sf::SoundBuffer* getSoundBuffer(std::string filename);

	// Font retrieval
	sf::Font* getFont(std::string filename);

	// Shader retrieval
	sf::Shader* getShader(std::string filename);

	// Determines optimal texture size, in powers of 2 (ie, 16x16, 32x32, etc)
	static sf::Vector2u getIdealTexSize(sf::Vector2u size);
	
	// Retrieves texture map
	std::map<std::string, TexData>* getTexMap() {
		return &texResources;
	};

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
	
private:
	// Reads data directly from directory file
	void loadFromFile();
	// Writes data to resource pack file
	void writeToPack();
	// Read data from the resource pack file
	void loadFromPack();

	// Mutex to prevent multiple simultaneous access of resources
	std::mutex m;

	// Maps containing resources
	std::map<std::string, TexData> texResources;
	std::map<std::string, sf::SoundBuffer> audioResources;
	std::map<std::string, FontData> fontResources;
	std::map<std::string, std::unique_ptr<sf::Shader>> shaderResources;

	// Name, data offset of levels in pack file
	std::map<std::string, int> levels;

	// List of level names
	std::vector<std::string> levelPaths;
	
	// Retrieves files
	template <class T>
	T* fetchResource(std::string filename, std::map<std::string, T> &map);

	// File metadata
	struct File {
		std::string name;
		std::string dir;
		int size;
	};
};

//shorthand for ResourceLoader::get()
ResourceLoader* RL();

#endif