#ifndef FILESTREAM_H
#define FILESTREAM_H

#include <fstream>

#include "SFML/System.hpp"

// Custom input stream for reading pack file
class FileStream : public sf::InputStream {

	std::ifstream *m_source = nullptr;

	int start, end;

public:
	explicit FileStream(std::ifstream *stream, int st, int en);

	sf::Int64 read(void* data, sf::Int64 size) override;
	sf::Int64 seek(sf::Int64 position) override;
	sf::Int64 tell() override;
	sf::Int64 getSize() override;
};

#endif