#include "GameFile.hpp"

#include "SFML/Graphics.hpp"

#include "Animation.hpp"

class TextureFile : public GameFile {
public:

	explicit TextureFile(std::string path, FileStream* str = nullptr) : GameFile(path, str) {
		fileType = FileType::TEXTURE;
	};

	const sf::Texture& get();
	const Animation* getAnimation(std::string animName);

private:
	bool in_loadFromFile(std::string path);
	bool in_loadFromStream(FileStream* str);
	void convertToData();

	sf::Texture tex;
	std::map<std::string, Animation> animations;

};
