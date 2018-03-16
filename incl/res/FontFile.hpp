#ifndef FONTFILE_H
#define FONTFILE_H

#include <SFML/Graphics.hpp>

#include "res/GameFile.hpp"

class FontFile : public GameFile {
public:

	FontFile(std::string path, FileStream* str = nullptr);

	const sf::Font& get();

private:
	bool loadFromFile(std::string path) override;
	bool loadFromStream(FileStream* str) override;
	void convertToData();

	sf::Font font;
};

#endif
