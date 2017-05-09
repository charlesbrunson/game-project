#include "GameFile.hpp"

#include "SFML/Graphics.hpp"

#include "Animation.hpp"

class TextureFile : public GameFile {
public:

	const sf::Texture& get() { 
		return tex; 
	};
	const Animation* getAnimation(std::string animName) { 
		auto f = animations.find(animName);
		return f != animations.end() ? &f->second : nullptr;
	};

private:
	bool in_loadFromFile(std::string path);
	bool in_loadFromStream(std::ifstream& str);
	void convertToData();

	sf::Texture tex;
	std::map<std::string, Animation> animations;

};
