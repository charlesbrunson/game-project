#include "FileStream.hpp"

FileStream::FileStream(std::ifstream *stream, int st, int en) : m_source(stream) {
	start = st;
	end = en;
	stream->seekg(start);
}

sf::Int64 FileStream::read(void* data, sf::Int64 size)
{
	m_source->read(static_cast<char*>(data), size);
	return m_source->gcount();
}

sf::Int64 FileStream::seek(sf::Int64 position)
{
	m_source->seekg(start + position, m_source->beg);

	return m_source->gcount();
}

sf::Int64 FileStream::tell()
{
	return (int)m_source->tellg() - start;
}

sf::Int64 FileStream::getSize()
{
	return end - start;
}
