#ifndef FONTFILE_H
#define FONTFILE_H

#include <SFML/Graphics.hpp>

#include "GameFile.hpp"

class FontFile : public GameFile {
public:

	explicit FontFile(std::string path, FileStream* str = nullptr);

	const sf::Font& get();

private:
	bool loadFromFile(std::string path) override;
	bool loadFromStream(FileStream* str) override;
	void convertToData();

	sf::Font font;
};

#endif