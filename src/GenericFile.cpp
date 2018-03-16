#include "res/GenericFile.hpp"
#include "res/ResourceLoader.hpp"

#include <fstream>
#include <sstream>

#include "util/StandardSize.hpp"

#include <assert.h>
#include <array>


GenericFile::GenericFile(std::string path, FileStream* str) : GameFile(path, str) {
	fileType = FileType::GENERIC;
	load(path, str);
};

const std::string& GenericFile::get() {
	// turn char array into string
	return data;
}

bool GenericFile::loadFromFile(std::string path) {

	clearData();

	std::ifstream reader(RL()->fileDir + path, std::ios_base::binary);
	if (reader.is_open()) {

		std::stringstream buffer;
		buffer << reader.rdbuf();

		data.reserve(reader.tellg());
		data = buffer.str();
		data.shrink_to_fit();

		reader.close();
		return true;
	}

	return false;
}

bool GenericFile::loadFromStream(FileStream* str) {

	clearData();
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

	return true;
}

void GenericFile::convertToData() {

}
