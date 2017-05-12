#include "FontFile.hpp"

#include <sstream>

#include "ResourceLoader.hpp"
#include "StandardSize.hpp"


FontFile::FontFile(std::string path, FileStream* str) : GameFile(path, str) {
	fileType = FileType::FONT;
	load(path, str);
};

const sf::Font& FontFile::get() {
	// turn char array into string
	return font;
}

bool FontFile::loadFromFile(std::string path) {
	if (font.loadFromFile(RL()->fileDir + path)) {

		// fix anti-aliasing
		for (int i = 8; i <= 128; i += 8)
			const_cast<sf::Texture&>(font.getTexture(i)).setSmooth(false);

		return true;
	}
	return false;
}

bool FontFile::loadFromStream(FileStream* str) {
	
	clearData();

	// font data needs to always be accessible by Font class
	int transferSize = 128;
	int remaining = str->getSize();

	char* d = new char[transferSize];

	while (remaining > 0) {
		int size = std::min(remaining, transferSize);
		str->read(d, size);
		data.append(d, size);
		remaining -= size;
	}

	delete[] d;

	// load font
	if (font.loadFromMemory(data.c_str(), str->getSize())) {

		// fix anti-aliasing
		for (int i = 8; i <= 128; i += 8)
			const_cast<sf::Texture&>(font.getTexture(i)).setSmooth(false);

		return true;
	}
	return false;
}

void FontFile::convertToData() {

	data.clear();

	std::stringstream out;

	// copy file to stream
	std::ifstream file(RL()->fileDir + filePath, std::ios::binary);

	if (file) {
		out << file.rdbuf();
		file.close();
	}
	else {
		return;
	}

	//copy stringstream to data
	std::string dstr = out.str();
	data.append(dstr.c_str(), dstr.size());

}

