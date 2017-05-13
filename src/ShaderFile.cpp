#include "GenericFile.hpp"
#include "ResourceLoader.hpp"

#include <fstream>
#include <sstream>

#include "StandardSize.hpp"

#include <assert.h>
#include <array>


ShaderFile::ShaderFile(sf::Shader::Type type, std::string path, FileStream* str) : GameFile(path, str) {
	fileType = FileType::SHADER;
	shaderType = type;
	load(path, str);
};

sf::Shader& ShaderFile::get() {
	return shader;
}

bool ShaderFile::loadFromFile(std::string path) {
	return shader.loadFromFile(RL()->fileDir + path, shaderType);
}

bool ShaderFile::loadFromStream(FileStream* str) {
	return shader.loadFromStream(*str, shaderType);
}

void ShaderFile::convertToData() {
	data.clear();

	std::stringstream out;

	// copy file to stream
	std::ifstream file(RL()->fileDir + filePath, std::ios::binary);

	if (file) {
		out << file.rdbuf();
		file.close();
	}

	//copy stringstream to data
	std::string dstr = out.str();
	data.append(dstr.c_str(), dstr.size());

}
