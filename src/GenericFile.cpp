#include "GenericFile.hpp"

#include <fstream>

#include "StandardSize.hpp"

#include <assert.h>
#include <array>

bool GenericFile::in_loadFromFile(std::string path) {

	clearData();

	std::ifstream reader;

	// open at end of file
	reader.open(path, std::ios_base::binary | std::ios_base::ate);
	if (reader.is_open()) {
		// determine filesize
		dataSize = reader.tellg();

		// init data
		data = new char[dataSize];

		// read file into data array
		reader.beg();
		while (!reader.eof()) {
			reader >> data;
		}
	}
	reader.close();

	return false;
}

bool GenericFile::in_loadFromStream(FileStream* str) {

	clearData();

	str->read(&dataSize, StdSizes::intSize);

	data = new char[dataSize];
	str->read(data, dataSize);

	return false;
}

void GenericFile::convertToData() {
	//file should be as data already, no conversion necessary
}