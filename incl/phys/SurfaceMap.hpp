#ifndef SURFACEMAP_H
#define SURFACEMAP_H

#include "Surface.hpp"

//contains level surfaces
//fast access by grid or iterator
//surface optimization
//combining or canceling out surfaces

class SurfaceMap {
public:

	SurfaceMap();
	SurfaceMap(unsigned int sizeX, unsigned int sizeY);
	void clear(unsigned int sizeX, unsigned int sizeY);

	void addSurface(Surface ss);
	void removeSurface(Surface ss);

	GridMap<std::vector<Surface*>> getSurfacesInGrid(GridVec2 place);
	std::vector<Surface>* getAllSurfaces();
	std::vector<Surface*> surfacesStartingAt(Point p);
	std::vector<Surface*> surfacesEndingAt(Point p);

private:

	void replaceSurface(Surface ss);
	void initGrid();

	std::vector<Surface> surfaces;
	GridMap<std::vector<Surface*>> surfGrid;
	sf::Vector2u size;
};

#endif
