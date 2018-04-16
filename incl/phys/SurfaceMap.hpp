#ifndef SURFACEMAP_H
#define SURFACEMAP_H

#include "Surface.hpp"

//contains level surfaces
//fast access by grid or iterator
//surface optimization
//combining or canceling out surfaces

#include "game/lvl/Tile.hpp"

class SurfaceMap {
friend class LevelSerializer;
public:

	SurfaceMap();
	SurfaceMap(unsigned int sizeX, unsigned int sizeY);
	void clear(unsigned int sizeX, unsigned int sizeY);

	void addTile(const Tile& t);
	void addShape(const TileShape& t, GridVec2 pos, bool firstSurfaceOnly = false);
	void removeShape(GridVec2 pos);

	std::vector<Surface>* getSurfacesInGrid(GridVec2 place);
	GridMap<std::vector<Surface>>* getAllSurfaces();
	//std::vector<Surface*> surfacesStartingAt(Point p);
	//std::vector<Surface*> surfacesEndingAt(Point p);

	struct Corner {
		bool valid = false;
		Vec2 position = Vec2();
		Vec2 normal = Vec2();
	};
	std::vector<Corner> corners;

private:

	void addSurface(Surface ss, GridVec2 pos, bool dontCull = false);
	void removeSurface(Surface ss, GridVec2 pos);

//	void replaceSurface(Surface ss);
//	void initGrid();

	GridMap<std::vector<Surface>> surfGrid;

	sf::Vector2u size;
};

#endif
