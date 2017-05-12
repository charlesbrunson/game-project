#ifndef LEVELFILE_H
#define LEVELFILE_H

#include <SFML/Graphics.hpp>

#include "GameFile.hpp"

class LevelFile : public GameFile {
public:

	LevelFile(std::string path, FileStream* str = nullptr);

	// location in packfile this level is located
	// levels are not stored in memory
	const int getPackPos();

private:
	bool loadFromFile(std::string path) override;
	bool loadFromStream(FileStream* str) override;
	void convertToData();
	
	int packPos = -1;
};

#endif