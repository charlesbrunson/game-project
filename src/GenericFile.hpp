#include "GameFile.hpp"

class GenericFile : public GameFile {
public:

	explicit GenericFile(std::string path, FileStream* str = nullptr) : GameFile(path, str) {
		fileType = FileType::GENERIC;
	};
	
	std::string get() {
		// turn char array into string
		return std::string(data, data + dataSize);
	}

private:
	bool in_loadFromFile(std::string path);
	bool in_loadFromStream(FileStream* str);
	void convertToData();
};
