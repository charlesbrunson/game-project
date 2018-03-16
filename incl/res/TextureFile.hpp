#ifndef TEXTUREFILE_H
#define TEXTUREFILE_H

#include "res/GameFile.hpp"

#include <SFML/Graphics.hpp>

#include "anim/Animation.hpp"

class TextureFile : public GameFile {
public:

	TextureFile(std::string path, FileStream* str = nullptr);

	const sf::Texture& get() const;
	Animation* getAnimation(std::string animName);

private:

	bool loadFromFile(std::string path) override;
	bool loadFromStream(FileStream* str) override;

	bool loadAnimationFile(const std::string& path);
	bool loadTileDataFile(const std::string& path);

	void convertToData() override;

	sf::Texture tex;
	std::map<std::string, Animation> animations;

	void linkAnimations();
};

#endif
