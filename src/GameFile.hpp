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
		FONT,
		LEVEL,
		SHADER,
		FILETYPE_COUNT
	};

	// loads information from file if FileStream not supplied
	GameFile(std::string path, FileStream* str = nullptr);
	virtual ~GameFile() = default;

	// factory methods for creating appropriate GameFile for file's type
	static GameFile* create(std::string path, FileStream* str = nullptr);

	// loads information from file if FileStream not supplied
	bool load(std::string path, FileStream* str = nullptr);

	const std::string* getData();
	int getDataSize();
	void clearData();

	FileType getType() {
		return fileType;
	}
	inline const std::string& getFilePath() { return filePath; };
	
protected:

	FileType fileType = FileType::UNKNOWN;

	std::string filePath = "";

	virtual bool loadFromFile(std::string path) = 0;
	virtual bool loadFromStream(FileStream* str) = 0;
	virtual void convertToData() = 0;
	
	std::string data;
};

#endif //GAMEFILE_H
