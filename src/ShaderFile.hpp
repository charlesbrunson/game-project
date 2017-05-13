#ifndef SHADERFILE_H
#define SHADERFILE_H

#include <SFML/Graphics.hpp>

#include "GameFile.hpp"

class ShaderFile : public GameFile {
public:
	ShaderFile(sf::Shader::Type type, std::string path, FileStream* str = nullptr);

	sf::Shader& get();

private:
	bool loadFromFile(std::string path) override;
	bool loadFromStream(FileStream* str) override;
	void convertToData();

	sf::Shader::Type shaderType;
	sf::Shader shader;
};

#endif
