#ifndef TEXTUREFILE_H
#define TEXTUREFILE_H

#include "GameFile.hpp"

#include "SFML/Graphics.hpp"

#include "Animation.hpp"

class TextureFile : public GameFile {
public:

	explicit TextureFile(std::string path, FileStream* str = nullptr) : GameFile(path, str) {
		fileType = FileType::TEXTURE;
		load(path, str);
	};

	const sf::Texture& get() const;
	const Animation& getAnimation(std::string animName) const;

private:

	bool loadFromFile(std::string path);
	bool loadFromStream(FileStream* str);
	void convertToData();

	sf::Texture tex;
	std::map<std::string, Animation> animations;

};

#endif