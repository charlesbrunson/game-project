
#include "ResourceLoader.hpp"

#include <fstream>
#include <assert.h>

//#if defined(_DEBUG) && defined(_WIN32)
//#include <Windows.h>
//#endif

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

// JSON parser
#include "json.h"

#include "FileStream.hpp"
#include "StandardSize.hpp"
#include "Log.hpp"


ResourceLoader ResourceLoader::resourceLoader;

ResourceLoader* RL() {
	return ResourceLoader::get();
}

// File directory constants
const std::string ResourceLoader::fileDir = "data/";
const std::string ResourceLoader::fileIndex = "data/index.txt";
const std::string ResourceLoader::packName = "data.pck";

bool ResourceLoader::loadResources() {

	//loadFromFile();

	//#ifdef _DEBUG
	// Create resource pack file
	writeToPack();
	//#endif

	// Load resources from pack file
	return loadFromPack();
}


void ResourceLoader::writeToPack() {
	// Read file paths from index.txt, transfer those files to resource pack file

	

	std::ofstream packWriter;

	std::vector<std::pair<std::string, GameFile*>> files;

	std::ifstream indexReader(fileIndex);
	if (indexReader.is_open()) {
		bool err = false;

		// index file is in json
		Json::Value root;
		indexReader >> root;

		if (root.isArray()) {
			for (auto i = root.begin(); i != root.end() && !err; i++) {

				// create file
				GameFile* nFile = GameFile::create(i->asString());

				if (!nFile) {
					err = true;
					break;
				}
				else {
					files.push_back(std::make_pair(i->asString(), nFile));
				}

			}
		}
		else {
			err = true;
		}

		assert(!err);
	}
	indexReader.close();

	// Start writing header for pack file
	packWriter.open(packName, std::ios_base::binary);
	if (packWriter.is_open()) {
		int fileSeek = 0;

		int headerSize = 0;
		std::stringstream headerStream;

		// write file count
		int fileCount = files.size();
		headerStream.write((char*)&fileCount, StdSizes::intSize);

		// write meta data for files
		for (auto i = files.begin(); i != files.end(); i++) {

			// Write file name length
			int fileNameLength = i->first.length();
			headerStream.write((char*)&fileNameLength, StdSizes::intSize);

			// Write file name
			for (int t = 0; t < fileNameLength; t++)
				headerStream.write((char*)&i->first.at(t), StdSizes::charSize);

			// Write file size
			int dataSize = i->second->getDataSize();
			headerStream.write((char*)&dataSize, StdSizes::intSize);

			// Write file seek
			headerStream.write((char*)&fileSeek, StdSizes::intSize);
			fileSeek += dataSize;
		}

		// write header length and content
		std::string headerData = headerStream.str();
		packHeaderSize = headerData.size() + StdSizes::intSize;

		packWriter.write((char*)&packHeaderSize, StdSizes::intSize);
		packWriter.write(headerData.c_str(), packHeaderSize);

		// Write files to resource pack file
		packWriter.seekp(packHeaderSize);
		// Loop through files
		for (auto i = files.begin(); i != files.end(); i++) {

			//write files
			const int max_size = 128;
			int remainingSize = i->second->getDataSize();

			char * ptr = (char*)i->second->getData()->c_str();

			while (remainingSize > 0) {

				int size = std::min(remainingSize, max_size);

				packWriter.write(ptr, size);
				remainingSize -= size;
				ptr += size;
			}
			i->second->clearData();
		}
	}

	//clean up files
	for (auto i = files.begin(); i != files.end(); i++)
		delete i->second;

	files.clear();

	//std::cout << "test\n";

}

// Read file from pack
bool ResourceLoader::loadFromPack() {

	std::ifstream packReader(packName, std::ios_base::binary);
	bool err = false;
	if (packReader.is_open()) {

		// get length of header
		packReader.read((char*)&packHeaderSize, StdSizes::intSize);

		int fileCount;
		packReader.read((char*)&fileCount, StdSizes::intSize);

		for (int i = 0; i < fileCount; i++) {

			int fileNameLength;
			std::string fileName = "";
			int fileSize;
			int fileSeek;

			packReader.read((char*)&fileNameLength, StdSizes::intSize);

			for (int t = 0; t < fileNameLength; t++) {
				char c;
				packReader.read((char*)&c, StdSizes::charSize);
				fileName += c;
			}

			packReader.read((char*)&fileSize, StdSizes::intSize);
			packReader.read((char*)&fileSeek, StdSizes::intSize);

			//hold header position
			int headerPos = packReader.tellg();

			//load file respective of type
			int start = packHeaderSize + fileSeek;
			int end = start + fileSize;

			GameFile* file = GameFile::create(fileName, &FileStream(&packReader, start, end));

			// add file
			if (file != nullptr) {
				switch (file->getType()) {
				case GameFile::FileType::UNKNOWN: err = true; break;
				case GameFile::FileType::GENERIC: generics.insert(std::make_pair(fileName, (GenericFile*)file)); break;
				case GameFile::FileType::TEXTURE: textures.insert(std::make_pair(fileName, (TextureFile*)file)); break;
				case GameFile::FileType::FONT: fonts.insert(std::make_pair(fileName, (FontFile*)file)); break;
				case GameFile::FileType::LEVEL: levels.insert(std::make_pair(fileName, (LevelFile*)file)); break;
				default: err = true;  break;
				}
			}
			else {
				delete file;
				err = true;
			}

			//return to header
			packReader.seekg(headerPos, std::ios_base::beg);

			if (err) break;
		}

		loaded = true && !err;
	}

	return loaded;
}


void ResourceLoader::dumpResources() {
	//Generics
	for (auto i = generics.begin(); i != generics.end(); i++) {
		delete i->second;
	}
	generics.clear();

	// Textures
	for (auto i = textures.begin(); i != textures.end(); i++) {
		delete i->second;
	}
	textures.clear();

	// Audio

	// Fonts
	for (auto i = fonts.begin(); i != fonts.end(); i++) {
		delete i->second;
	}
	fonts.clear();

	// Levels
	for (auto i = levels.begin(); i != levels.end(); i++) {
		delete i->second;
	}
	levels.clear();

	// Shaders

	loaded = false;
}

const std::string& ResourceLoader::getGeneric(std::string filename) {
	std::lock_guard<std::mutex> lock(m);

	auto i = generics.find(filename);
	assert(i != generics.end());
	return i->second->get();
}

const TextureFile& ResourceLoader::getTexFile(std::string filename) {
	std::lock_guard<std::mutex> lock(m);

	auto i = textures.find(filename);
	assert(i != textures.end());

	return *i->second;
}

const sf::Texture& ResourceLoader::getTexture(std::string filename) {
	return getTexFile(filename).get();
}

const sf::Font& ResourceLoader::getFont(std::string filename) {
	std::lock_guard<std::mutex> lock(m);

	auto i = fonts.find(filename);
	assert(i != fonts.end());

	return i->second->get();
}
sf::Shader* ResourceLoader::getShader(std::string filename) {
	//TODO
	return nullptr;
}

int ResourceLoader::getLevelOffset(std::string lvlname) {
	std::lock_guard<std::mutex> lock(m);

	auto i = levels.find(lvlname);
	assert(i != levels.end());

	return i->second->getPackPos();
}