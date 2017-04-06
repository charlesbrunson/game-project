
#include "ResourceLoader.hpp"

#include <fstream>
#include <math.h>
#include <thread>

#if _DEBUG && _WIN32
#include <Windows.h>
#endif

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

// JSON parser
#include "json.h"

#include "Globals.hpp"
#include "LevelLoader.hpp"
#include "TileProperty.hpp"
#include "Log.hpp"


ResourceLoader ResourceLoader::resourceLoader;

ResourceLoader* RL() {
	return ResourceLoader::get();
}

std::string removeChar(std::string s, char c);
std::string removeWhitespace(std::string s);

const std::string ResourceLoader::fileTypes[TYPE_COUNT] = {
	"sprite",
	"tileset",
	"sound",
	"font",
	"level",
	"shaders"
};

// File directory constants
const std::string ResourceLoader::fileDir = "data/";
const std::string ResourceLoader::fileIndex = "data/index.txt";
const std::string ResourceLoader::packName = "data.pck";

// Allotted space in bytes for resource pack file header data, which contains information for faster navigation
const int ResourceLoader::packHeaderSize = 1024;

// Custom input stream for reading pack file
class FileStream : public sf::InputStream {

	std::ifstream *m_source;

	int start, end;

public:
	explicit FileStream(std::ifstream *stream, int st, int en) : m_source(stream) {
		start = st;
		end = en;
	}

	sf::Int64 read(void* data, sf::Int64 size) override
	{
		m_source->read(static_cast<char*>(data), size);
		return m_source->gcount();
	}
	sf::Int64 seek(sf::Int64 position) override
	{
		m_source->seekg(start + position, m_source->beg);

		return m_source->gcount();
	}
	sf::Int64 tell() override
	{
		return (int)m_source->tellg() - start;
	}
	sf::Int64 getSize() override
	{
		return end - start;
	}
};

bool ResourceLoader::loadResources() {

	//loadFromFile();

#ifdef _DEBUG
	// Create resource pack file
	writeToPack();
#endif

	// Load resources from pack file
	return loadFromPack();
}


void ResourceLoader::writeToPack() {
	// Read file paths from index.txt, transfer those files to resource pack file

	std::ifstream indexReader;
	std::ifstream fileReader;
	std::ofstream packWriter;
	
	// Mapped files by type
	std::map<std::string, std::vector<File>> files;

	// Load tileset data
	TileProperty::initTileData();

	// Open index file, which gives the file paths of everything that needs to be loaded
	indexReader.open(fileDir + "index.txt");
	if (indexReader.is_open()) {

		// Index file is JSON formatted, use parser
		Json::Value root;
		indexReader >> root;

		// Files are separated by type
		for (auto i = root.begin(); i != root.end(); i++) {
			std::string fileTypeName = i.name();
			Json::Value fileTypeInfo = *i;

			bool filetypeValid = false;
			for (auto i = 0; i < TYPE_COUNT; i++) {
				if (fileTypes[i] == fileTypeName) {
					filetypeValid = true;
					break;
				}
			}

			// Validate file type
			if (filetypeValid) {
				
				std::string directory = fileTypeInfo.get("directory", "").asString();

				// Add file type if it doesn't already exist
				auto i = files.find(fileTypeName);
				if (i == files.end()) {
					files.insert(std::make_pair(fileTypeName, std::vector<File>()));
				}

				// Start looking through files
				Json::Value fileList = fileTypeInfo.get("files", Json::arrayValue);
				for (auto file = fileList.begin(); file != fileList.end(); file++) {

					// This file's filename
					std::string filename = file->asString();

					// Try and compile TMX files as native LVL files before achiving
					if (fileTypeName == fileTypes[LEVEL]) {
						bool toCompile = false;
						
#if _DEBUG && _WIN32
						// Auto recompile .lvl file if older than the associated .tmx file
						// Windows only for now
						{
							std::string lvlName = fileDir + directory + filename;
							lvlName.erase(lvlName.find_first_of(".lvl"), lvlName.length());
							lvlName.append(".tmx");

							HANDLE lvlFileHandle = CreateFile((fileDir + directory + filename).c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
							HANDLE tmxFileHandle = CreateFile(lvlName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

							FILETIME tmxWriteTime,
								lvlWriteTime;

							bool tmxExists = GetFileTime(tmxFileHandle, NULL, NULL, &tmxWriteTime) != 0;
							bool lvlExists = GetFileTime(lvlFileHandle, NULL, NULL, &lvlWriteTime) != 0;

							CloseHandle(lvlFileHandle);
							CloseHandle(tmxFileHandle);

							toCompile = tmxExists && (!lvlExists || (lvlExists && CompareFileTime(&lvlWriteTime, &tmxWriteTime) < 0));
						}
#endif
						// If the .lvl file needs to be recompiled, do so
						if (toCompile || Gameplay_Globals::Debug::forceCompile) {

							// Get full file path from directory
							std::string tmxFile = fileDir + directory + filename;
							Log::msg(tmxFile + " - needs recompile\n");
							tmxFile.erase(tmxFile.length() - 4, tmxFile.length());
							tmxFile.erase(0, fileDir.length());

							// Pass to level tmx compiler
							LevelLoader::compileTMXFile(tmxFile);
						}
					}


					// Determine filesize
					int fileSize = 0;
					fileReader.open(fileDir + directory + filename, std::ios_base::binary | std::ios_base::ate);
					if (fileReader.is_open()) {
						fileSize = fileReader.tellg();
					}
					fileReader.close();

					// Store file information for header
					if (fileSize > 0) {
						File f;
						f.name = filename;
						f.dir = directory;
						f.size = fileSize;
						
						files.at(fileTypeName).push_back(f);
					}
				}
			}
		}
	}
	// Close index fle
	indexReader.close();
	
	// Start writing header for pack file
	packWriter.open(packName, std::ios_base::binary);
	if (packWriter.is_open()) {
		int fileSeek = 0;

		// Write type count
		int fileTypeCount = files.size();
		packWriter.write((char*)&fileTypeCount, StdSizes::intSize);
		for (auto i = files.begin(); i != files.end(); i++) {

			// Write file type string length
			int fileTypeStringLength = i->first.length();
			packWriter.write((char*)&fileTypeStringLength, StdSizes::intSize);

			// Write file type string
			for (int r = 0; r < fileTypeStringLength; r++)
				packWriter.write((char*)&i->first.at(r), StdSizes::charSize);

			// Write file count of type
			int fileCountOfType = i->second.size();
			packWriter.write((char*)&fileCountOfType, StdSizes::intSize);

			for (auto j = i->second.begin(); j != i->second.end(); j++) {

				// Write file name length
				int fileNameLength = j->name.length();
				packWriter.write((char*)&fileNameLength, StdSizes::intSize);

				// Write file name
				for (int t = 0; t < fileNameLength; t++)
					packWriter.write((char*)&j->name.at(t), StdSizes::charSize);

				// Write file size
				packWriter.write((char*)&j->size, StdSizes::intSize);

				// Write file seek
				packWriter.write((char*)&fileSeek, StdSizes::intSize);
				fileSeek += j->size;
			}
		}

		// Write files to resource pack file
		packWriter.seekp(packHeaderSize);
		// Loop by file type
		for (auto i = files.begin(); i != files.end(); i++) {
			// Loop by file of this type
			for (auto j = i->second.begin(); j != i->second.end(); j++) {

				// Open file, write contents to end of pack file
				fileReader.open(fileDir + j->dir + j->name, std::ios_base::binary);
				if (fileReader.is_open()) {

					const int max_size = 128;
					int remainingSize = j->size;

					char * data = new char[max_size];

					while (!fileReader.eof() && remainingSize > 0) {

						int size = std::min(remainingSize, max_size);
						fileReader.read(data, size);
						packWriter.write(data, size);
						remainingSize -= size;
					}

					delete[] data;
				}
				fileReader.close();
			}
		}
	}
	packWriter.close();
}

// Read file from pack
bool ResourceLoader::loadFromPack() {
	
	std::ifstream packReader(packName, std::ios_base::binary);
	if (packReader.is_open()) {

		// Read type count
		int fileTypeCount;
		packReader.read((char*)&fileTypeCount, StdSizes::intSize);

		for (int i = 0; i < fileTypeCount; i++) {

			// Read file type string length
			int fileTypeStringLength;
			packReader.read((char*)&fileTypeStringLength, StdSizes::intSize);

			// Read file type string
			std::string type = "";
			for (int r = 0; r < fileTypeStringLength; r++) {
				char c;
				packReader.read((char*)&c, StdSizes::charSize);
				type += c;
			}

			// Read file count of type
			int fileCountOfType;
			packReader.read((char*)&fileCountOfType, StdSizes::intSize);

			for (int j = 0; j < fileCountOfType; j++) {

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
				//go to file position
				packReader.seekg(packHeaderSize + fileSeek, std::ios_base::beg);

				//load file respective of type
				if (type == fileTypes[SPRITE]) {

					TexData texspr;
					FileStream stream(&packReader, packReader.tellg(), (int)packReader.tellg() + fileSize);

					if (texspr.t.loadFromStream(stream)) {
						texResources.insert(std::make_pair(fileName, texspr));
					}
				}
				else if (type == fileTypes[TILESET]) {

					TexData textile;
					FileStream stream(&packReader, packReader.tellg(), (int)packReader.tellg() + fileSize);

					if (textile.t.loadFromStream(stream)) {
						textile.isTileset = true;
						texResources.insert(std::make_pair(fileName, textile));
					}
				}
				else if (type == fileTypes[SOUND]) {

					sf::SoundBuffer sound;
					char * soundData = new char[fileSize];

					packReader.read(soundData, fileSize);

					if (sound.loadFromMemory(soundData, fileSize)) {
						audioResources.insert(std::make_pair(fileName, sound));
					}

					delete[] soundData;

				}
				else if (type == fileTypes[FONT]) {
					// Fonts need constant access to stream
					fontResources.emplace(std::pair<std::string, FontData>(fileName, std::move(FontData(fileSize, &packReader))));

					FontData *d = &fontResources.at(fileName);

					if (d->valid) {

						// Process multiple point sizes, to removes blurry antialiasing
						for (int i = 8; i <= 128; i += 8)
							const_cast<sf::Texture&>(d->font.getTexture(i)).setSmooth(false);

					}
					else {
						delete[] d->fontData;
						fontResources.erase(fileName);
					}

				}
				else if (type == fileTypes[LEVEL]) {
					levels.insert(std::make_pair(fileName, fileSeek));
				}
				else if (type == fileTypes[SHADERS]) {

					bool isFrag = fileName.find(".frag") != std::string::npos;
					bool isVert = fileName.find(".vert") != std::string::npos;

					if (isFrag != isVert) {
						char * shaderData = new char[fileSize + 1];
						packReader.read(shaderData, fileSize);
						shaderData[fileSize] = '\0';
						
						sf::Shader::Type t = isFrag ? sf::Shader::Fragment : sf::Shader::Vertex;

						auto s = shaderResources.emplace(fileName, std::make_unique<sf::Shader>());
						sf::Shader *shader = s.first->second.get();

						bool test = shader->loadFromMemory(shaderData, t);

						if (test) {
							shader->setUniform("u_texture", sf::Shader::CurrentTexture);
						}
						else {
							shaderResources.erase(fileName);
						}

						delete[] shaderData;
					}
					else {
						assert(false);
					}

				}
				//return to header
				packReader.seekg(headerPos, std::ios_base::beg);
			}
		}
		loaded = true;
	}
	else {
		Log::msg("Failed to load data.pck file\n");
	}

	packReader.close();	
	return loaded;
}

sf::Vector2u ResourceLoader::getIdealTexSize(sf::Vector2u size) {
	sf::Vector2u newSize;
	int i, j;
	for (i = 0; pow(2, i) < size.x; i++);
	for (j = 0; pow(2, j) < size.y; j++);

	newSize = sf::Vector2u(pow(2, i), pow(2, j));
	return newSize;
};

void ResourceLoader::dumpResources() {
	texResources.clear();
	audioResources.clear();

	for (auto i = fontResources.begin(); i != fontResources.end(); i++)
		delete[] i->second.fontData;

	fontResources.clear();
	levels.clear();
	shaderResources.clear();
	loaded = false;
}

bool ResourceLoader::isTextureTileset(std::string filename) {
	std::lock_guard<std::mutex> lock(m);

	TexData* i = fetchResource<TexData>(filename, texResources);
	if (i != nullptr)
		return i->isTileset;
	else
		return false;
}

sf::Texture* ResourceLoader::getTexture(std::string filename) {
	std::lock_guard<std::mutex> lock(m);

	auto i = fetchResource<TexData>(filename, texResources);
	if (i != nullptr)
		return &i->t;
	else
		return nullptr;
}
sf::SoundBuffer* ResourceLoader::getSoundBuffer(std::string filename) {
	std::lock_guard<std::mutex> lock(m);
	return fetchResource<sf::SoundBuffer>(filename, audioResources);
}
sf::Font* ResourceLoader::getFont(std::string filename) {
	std::lock_guard<std::mutex> lock(m);
	auto i = fetchResource<FontData>(filename, fontResources);
	if (i != nullptr)
		return &i->font;
	else
		return nullptr;
}

sf::Shader* ResourceLoader::getShader(std::string filename) {
	std::lock_guard<std::mutex> lock(m);
	auto i = fetchResource<std::unique_ptr<sf::Shader>>(filename, shaderResources);
	if (i != nullptr)
		return i->get();
	else
		return nullptr;
}

template <class T>
T* ResourceLoader::fetchResource(std::string filename, std::map<std::string, T> &map) {
	auto i = map.find(filename);
	if (i != map.end()) {
		return &i->second;
	}
	else {
		// Fetch failure
		return nullptr;
	}
}

std::string removeChar(std::string s, char c) {
	return std::string(s.begin(), std::remove(s.begin(), s.end(), c));
}

std::string removeWhitespace(std::string s) {
	return std::string(s.begin(), std::remove_if(s.begin(), s.end(), isspace));
}
