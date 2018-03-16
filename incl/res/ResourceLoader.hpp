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

#include "res/GenericFile.hpp"
#include "res/TextureFile.hpp"
#include "res/FontFile.hpp"
#include "res/LevelFile.hpp"
#include "res/ShaderFile.hpp"

// Singleton
// Loads and stores assets for program
class ResourceLoader {

	static ResourceLoader resourceLoader;

	ResourceLoader() = default;
	~ResourceLoader() = default;

public:

	//Singleton access
	static ResourceLoader* get();

	// Allocating and Deallocating resources
	bool loadResources();
	void dumpResources();

	bool reloadTextures();

	// Resource retrieval
	TextureFile& 	   getTexFile     (const std::string& filename);
	const sf::Texture& getTexture     (const std::string& filename);
	const std::string& getGeneric     (const std::string& filename);
	const sf::Font&	   getFont        (const std::string& filename);
	sf::Shader*        getShader      (const std::string& filename);

	// modifies stream to read level data
	bool openLevelData(const std::string& filename, std::ifstream* str);
	
	// Resource directory and pack file constants
	const static std::string fileDir;
	const static std::string fileIndex;
	const static std::string packName;

	inline int getPackHeaderSize() { return packHeaderSize; };
	inline bool isLoaded() { return loaded; };

private:
	int packHeaderSize;

	bool loaded = false;
	
	// Writes data to resource pack file
	bool writeToPack();

	// Reads data directly from directory file
	// not implemented, probably depreciated
	bool loadFromFile();
	// Read data from the resource pack file
	bool loadFromPack();

	bool addFile(const std::string& fileName, GameFile* file);

	// Mutex to prevent multiple simultaneous access of resources
	std::mutex m;

	// Maps containing resources of verious types
	std::map<std::string, TextureFile*> textures;
	std::map<std::string, GenericFile*> generics;
	std::map<std::string, FontFile*> fonts;
	std::map<std::string, LevelFile*> levels;
	std::map<std::string, ShaderFile*> shaders;

	template<class T>
	T* getResource(std::map<std::string, T*>& map, const std::string& name);
};

//shorthand for ResourceLoader::get()
ResourceLoader* RL();

#endif
