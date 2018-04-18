#ifndef SURFACEMAP_H
#define SURFACEMAP_H

#include "phys/Surface.hpp"
#include "phys/Corner.hpp"

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
	std::vector<Corner>* getCornersInGrid(GridVec2 place);
	GridMap<std::vector<Surface>>* getAllSurfaces();
	GridMap<std::vector<Corner>>* getAllCorners();

private:

	void addSurface(Surface ss, GridVec2 pos, bool dontCull = false);
	void removeSurface(Surface ss, GridVec2 pos);

	GridMap<std::vector<Surface>> surfGrid;
	GridMap<std::vector<Corner>>  cornerGrid;

	sf::Vector2u size;
};

#endif
