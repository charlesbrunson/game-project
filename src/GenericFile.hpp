#ifndef GENERICFILE_H
#define GENERICFILE_H

#include "GameFile.hpp"

class GenericFile : public GameFile {
public:

	explicit GenericFile(std::string path, FileStream* str = nullptr) : GameFile(path, str) {
		fileType = FileType::GENERIC;
		load(path, str);
	};

	const std::string& get() {
		// turn char array into string
		return data;
	}

private:
	bool loadFromFile(std::string path) override;
	bool loadFromStream(FileStream* str) override;
	void convertToData();
};

#endif
