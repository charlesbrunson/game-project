#ifndef LEVELFILE_H
#define LEVELFILE_H

#include <SFML/Graphics.hpp>

#include "res/GameFile.hpp"

class LevelFile : public GameFile {
public:

	LevelFile(std::string path, FileStream* str = nullptr);
	
	// modifies stream to be at beginning of level data
	// determined by if pack position is defined
	bool openData(std::ifstream* str);

private:
	bool loadFromFile(std::string path) override;
	bool loadFromStream(FileStream* str) override;
	void convertToData();
	
	int packPos = -1;

};

#endif
