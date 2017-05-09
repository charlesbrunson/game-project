#ifndef GAMEFILE_H
#define GAMEFILE_H

#include <vector>
#include <string>

class GameFile {
public:
	GameFile(std::string path) { 
		filePath = path;
		loadFromFile(path); 
	};
	GameFile(std::string fileName, std::ifstream& str) { 
		filePath = fileName;
		loadFromStream(str); 
	};

	bool loadFromFile(std::string path) {
		bool r = in_loadFromFile(path);

		if (r)
			convertToData();

		return r;
	};
	bool loadFromStream(std::ifstream& str) {
		bool r = in_loadFromStream(str);

		if (r)
			convertToData();

		return r;
	};
	const std::vector<char>& getData() {
		if (data.empty())
			convertToData();

		return data;
	}

	void clearData();

	bool valid();

protected:

	std::string filePath = "";

	virtual bool in_loadFromFile(std::string path) = 0;
	virtual bool in_loadFromStream(std::ifstream& str) = 0;
	virtual void convertToData() = 0;

	bool isValid = false;

	std::vector<char> data;
};

#endif //GAMEFILE_H