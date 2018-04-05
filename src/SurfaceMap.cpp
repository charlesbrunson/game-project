#include "phys/SurfaceMap.hpp"
#include "util/Log.hpp"

SurfaceMap::SurfaceMap() {
	size = sf::Vector2u(0, 0);
}

SurfaceMap::SurfaceMap(unsigned int sizeX, unsigned int sizeY) {
	size = sf::Vector2u(sizeX, sizeY);
}

void SurfaceMap::clear(unsigned int sizeX, unsigned int sizeY) {
	surfGrid.clear();	
	size = sf::Vector2u(sizeX, sizeY);
};

void SurfaceMap::addTile(const Tile& t) {
	if (t.tileProperty == TileProperty::tileProps::TILE_NOCOLLISION)
		return;

	addShape(t.shape, t.gridPos, t.tileProperty == TileProperty::tileProps::TILE_ONEWAY);
};

void SurfaceMap::addShape(const TileShape& t, GridVec2 pos, bool firstSurfaceOnly) {

	for (int i = 0; i + 1 < t.shapeNumCorners && i < t.shapeMaxCorners; i++) {

		Surface s(t.vertexes[i], t.vertexes[i + 1]);

		addSurface(s, pos);

		//only get the top surface
		if (firstSurfaceOnly) {
			break;
		}
	}
	if (!firstSurfaceOnly) {
		Surface s(t.vertexes[t.shapeNumCorners - 1], t.vertexes[0]);

		addSurface(s, pos);
	}
};

void SurfaceMap::removeShape(GridVec2 pos) {

	auto vecf = surfGrid.find(pos);
	if (vecf == surfGrid.end()) {
		return;
	}
	std::vector<Surface>* vec = &vecf->second;

	for (auto i = vec->begin(); i != vec->end(); i++) {
		removeSurface(*i, pos);
	}
};

void SurfaceMap::addSurface(Surface ss, GridVec2 pos) {
	auto vecf = surfGrid.find(pos);
	if (vecf == surfGrid.end()) {
		vecf = surfGrid.insert(std::make_pair(pos, std::vector<Surface>())).first;
	}
	std::vector<Surface>* vec = &vecf->second;

	Surface worldSurf = ss;
	worldSurf.line.start *= tileSpacing;
	worldSurf.line.end *= tileSpacing;
	worldSurf.line.start += (Vec2(pos) * tileSpacing);
	worldSurf.line.end += (Vec2(pos) * tileSpacing);


	static auto checkAdjacentSurfaces = [&worldSurf](Surface& comp, GridVec2 adjPos, GridMap<std::vector<Surface>>& grid) -> bool {

		auto vec = grid.find(adjPos);
		if (vec == grid.end()) {
			return true;
		}
		else {
			for (auto s = vec->second.begin(); s != vec->second.end(); s++) {
				Line sr = s->line.reverse();
				if (sr == comp.line) {
					vec->second.erase(s);
					return false;
				}

				if (sr.start == comp.line.start) {
					if (Math::dist(sr) > Math::dist(comp.line) && sr.hasPoint(comp.line.end)) {
						s->line.end = comp.line.end;
						return false;
					}
					else if (Math::dist(sr) < Math::dist(comp.line) && comp.line.hasPoint(sr.end)) {
						comp.line.start = sr.end;
						vec->second.erase(s);
						return true;
					}
				}
				else if (sr.end == comp.line.end) {
					if (Math::dist(sr) > Math::dist(comp.line) && sr.hasPoint(comp.line.start)) {
						s->line.start = comp.line.start;
						return false;
					}
					else if (Math::dist(sr) < Math::dist(comp.line) && comp.line.hasPoint(sr.start)) {
						comp.line.end = sr.start;
						vec->second.erase(s);
						return true;
					}
				}
			}
		}
		
		return true;
	};

	//line is on the border of the grid
	bool toAdd = true;
	if (ss.line.start.y == ss.line.end.y && ss.line.start.y == 0.f) {
		//check above
		toAdd = checkAdjacentSurfaces(worldSurf, pos + GridVec2(0, -1), surfGrid);

	}
	else if (ss.line.start.x == ss.line.end.x && ss.line.start.x == 1.f) {
		//check right
		toAdd = checkAdjacentSurfaces(worldSurf, pos + GridVec2(1, 0), surfGrid);

	}
	else if (ss.line.start.y == ss.line.end.y && ss.line.start.y == 1.f) {
		//check below
		toAdd = checkAdjacentSurfaces(worldSurf, pos + GridVec2(0, 1), surfGrid);

	}
	else if (ss.line.start.x == ss.line.end.x && ss.line.start.x == 0.f) {
		//check left
		toAdd = checkAdjacentSurfaces(worldSurf, pos + GridVec2(-1, 0), surfGrid);
	}

	if (toAdd) {
		vec->push_back(worldSurf);
	}

}

void SurfaceMap::removeSurface(Surface ss, GridVec2 pos) {
	//Log::msg("WHOOP");
}

std::vector<Surface>* SurfaceMap::getSurfacesInGrid(GridVec2 place) {
	return surfGrid.find(place) != surfGrid.end() ? &surfGrid.at(place) : nullptr;	
}

GridMap<std::vector<Surface>>* SurfaceMap::getAllSurfaces() {
	return &surfGrid;
}

/*
std::vector<Surface*> SurfaceMap::surfacesStartingAt(Point p) {

}

std::vector<Surface*> SurfaceMap::surfacesEndingAt(Point p) {

}

void SurfaceMap::initGrid() {

}
*/

