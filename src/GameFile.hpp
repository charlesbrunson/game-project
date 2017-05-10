#ifndef GAMEFILE_H
#define GAMEFILE_H

#include <vector>
#include <string>

#include "FileStream.hpp"

class GameFile {
public:
	enum FileType {
		UNKNOWN = -1,
		GENERIC = 0,
		TEXTURE,
		FILETYPE_COUNT
	};
	
	// loads information from file if FileStream not supplied
	explicit GameFile(std::string path, FileStream* str = nullptr);

	// factory methods for creating appropriate GameFile for file's type
	static GameFile* create(std::string path, FileStream* str = nullptr);

	// loads information from file if FileStream not supplied
	bool load(std::string path, FileStream* str = nullptr);

	const char* getData();
	const int getDataSize();
	void clearData();

	FileType getType() {
		return fileType;
	}

	bool valid();

protected:

	FileType fileType = FileType::UNKNOWN;

	std::string filePath = "";

	virtual bool in_loadFromFile(std::string path) = 0;
	virtual bool in_loadFromStream(FileStream* str) = 0;
	virtual void convertToData() = 0;

	bool validData = false;

	//std::vector<char> data;
	char *data;
	int dataSize = 0;
};

#endif //GAMEFILE_H