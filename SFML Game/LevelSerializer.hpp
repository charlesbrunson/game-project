#ifndef LEVELSERIALIZER_H
#define LEVELSERIALIZER_H

#include "GridVector.hpp"
#include "Tile.hpp"
#include "Level.hpp"


class LevelSerializer {
public:
	//do not create an object of this class
	virtual ~LevelSerializer() = 0;

	//writing
	static std::ostream& writeLevel(std::ostream& os, Level& lvl);

	//reading
	static std::istream& readLevel(std::istream& is, Level& lvl);

private:
	//writing
	static void writeFloatRect(std::ostream& os, const sf::FloatRect rect);
	static void writeTrigger(std::ostream& os, const Level::Trigger trigger);
	static void writeLvlCol(std::ostream& os, const Level::LevelCollision col);
	static void writeTileLayer(std::ostream& os, const TileLayer& layer);
	static void writeTileMap(std::ostream& os, const std::map<GridVector, Tile> &tilemap);
	static void writeColVec(std::ostream& os, const std::vector<sf::FloatRect> &vec);
	static void writeTrans(std::ostream& os, const Transition t);
	static void writeTransVec(std::ostream& os, const std::vector<Transition> &vec);
	static void writeTriggerVec(std::ostream& os, const std::vector<Level::Trigger> &vec);

	//reading
	static void readFloatRect(std::istream& is, sf::FloatRect &rect);
	static void readTrigger(std::istream& is, Level::Trigger &trigger);
	static void readLvlCol(std::istream& is, Level::LevelCollision &col);
	static void readTileLayer(std::istream& is, TileLayer& layer);
	static void readTileMap(std::istream& is, std::map<GridVector, Tile> &tilemap);
	static void readColVec(std::istream& is, std::vector<sf::FloatRect> &vec);
	static void readTrans(std::istream& is, Transition &t);
	static void readTransVec(std::istream& is, std::vector<Transition> &vec);
	static void readTriggerVec(std::istream& is, std::vector<Level::Trigger> &vec);

	static void write(int i, std::ostream& os);
	static void write(float i, std::ostream& os);
	static void write(bool i, std::ostream& os);
	static void write(char i, std::ostream& os);

	static void read(int &i, std::istream& is);
	static void read(float &i, std::istream& is);
	static void read(bool &i, std::istream& is);
	static void read(char &i, std::istream& is);
};

#endif