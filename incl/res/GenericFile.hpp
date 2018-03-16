#ifndef GENERICFILE_H
#define GENERICFILE_H

#include "res/GameFile.hpp"

class GenericFile : public GameFile {
public:

	GenericFile(std::string path, FileStream* str = nullptr);

	const std::string& get();

private:
	bool loadFromFile(std::string path) override;
	bool loadFromStream(FileStream* str) override;
	void convertToData();
};

#endif
