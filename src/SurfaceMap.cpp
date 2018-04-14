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

		s.line.start *= tileSpacing;
		s.line.end *= tileSpacing;
		s.line.start += (Vec2(pos) * tileSpacing);
		s.line.end += (Vec2(pos) * tileSpacing);

		addSurface(s, pos);

		//only get the top surface
		if (firstSurfaceOnly) {
			break;
		}
	}
	if (!firstSurfaceOnly) {
		Surface s(t.vertexes[t.shapeNumCorners - 1], t.vertexes[0]);

		s.line.start *= tileSpacing;
		s.line.end *= tileSpacing;
		s.line.start += (Vec2(pos) * tileSpacing);
		s.line.end += (Vec2(pos) * tileSpacing);

		addSurface(s, pos);
	}
	
	//shapeGrid.insert(std::make_pair(pos, std::make_pair(t, firstSurfaceOnly)));
};

void SurfaceMap::removeShape(GridVec2 pos) {

	/*
	auto vecf = shapeGrid.find(pos);
	if (vecf == shapeGrid.end()) {
		return;
	}
	TileShape& t = vecf->second.first;
	bool firstSurfaceOnly = vecf->second.second;


	for (int i = 0; i + 1 < t.shapeNumCorners && i < t.shapeMaxCorners; i++) {

		Surface s(t.vertexes[i], t.vertexes[i + 1]);

		s.line.start *= tileSpacing;
		s.line.end *= tileSpacing;
		s.line.start += (Vec2(pos) * tileSpacing);
		s.line.end += (Vec2(pos) * tileSpacing);

		removeSurface(s, pos);

		//only get the top surface
		if (firstSurfaceOnly) {
			break;
		}
	}
	if (!firstSurfaceOnly) {
		Surface s(t.vertexes[t.shapeNumCorners - 1], t.vertexes[0]);

		s.line.start *= tileSpacing;
		s.line.end *= tileSpacing;
		s.line.start += (Vec2(pos) * tileSpacing);
		s.line.end += (Vec2(pos) * tileSpacing);

		removeSurface(s, pos);
	}

	shapeGrid.erase(pos);
	*/
};

void SurfaceMap::addSurface(Surface ss, GridVec2 pos, bool dontCull) {

	static auto checkAdjacentSurfaces = [](Surface& comp, GridVec2 adjPos, GridMap<std::vector<Surface>>& grid) -> bool {

		auto vec = grid.find(adjPos);
		if (vec == grid.end()) {
			return true;
		}
		else {
			for (auto s = vec->second.begin(); s != vec->second.end(); s++) {
				Line sr = s->line.reverse();
				if (sr == comp.line) {
					vec->second.erase(s);
					if (vec->second.empty()){
						grid.erase(vec);
					}
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
						if (vec->second.empty()){
							grid.erase(vec);
						}
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
						if (vec->second.empty()){
							grid.erase(vec);
						}
						return true;
					}
				}
			}
		}
		
		return true;
	};

	//line is on the border of the grid
	bool toAdd = true;
	if (!dontCull) {
		toAdd = checkAdjacentSurfaces(ss, pos, surfGrid);
		if (toAdd && ss.line.isHorizontal()) {
			if (ss.line.start.y == pos.y * tileSpacing) {
				toAdd = checkAdjacentSurfaces(ss, pos + GridVec2(0, -1), surfGrid);
			}
			else if (ss.line.start.y == (pos.y + 1) * tileSpacing) {
				toAdd = checkAdjacentSurfaces(ss, pos + GridVec2(0, 1), surfGrid);
			}
		}
		else if (toAdd && ss.line.isVertical()) {
			if (ss.line.start.x == pos.x * tileSpacing) {
				toAdd = checkAdjacentSurfaces(ss, pos + GridVec2(-1, 0), surfGrid);
			}
			else if (ss.line.start.x == (pos.x + 1) * tileSpacing) {
				toAdd = checkAdjacentSurfaces(ss, pos + GridVec2(1, 0), surfGrid);
			}
		}
	}
	if (!toAdd) {
		return;
	}

	auto vecf = surfGrid.find(pos);
	if (vecf == surfGrid.end()) {
		vecf = surfGrid.insert(std::make_pair(pos, std::vector<Surface>())).first;
	}
	std::vector<Surface>* vec = &vecf->second;

	vec->push_back(ss);

	static auto tryConnect = [](Surface& a, Surface& b) -> void {
		if (a.line.isVertical() != b.line.isVertical())
			return;

		if ((a.line.getVector().x > 0.f) != (b.line.getVector().x > 0.f))
			return;

		Surface** startConn;
		Surface** endConn;
		Surface* start;
		Surface* end;
		if (a.line.start == b.line.end) {
			startConn = &a.startConn;
			start = &a;
			endConn = &b.endConn;
			end = &b;
		}
		else if (a.line.end == b.line.start) {
			startConn = &b.startConn;
			start = &b;
			endConn = &a.endConn;
			end = &a;
		}
		else return;

		if (*endConn == nullptr && *startConn == nullptr) {
			*endConn = start;	
			*startConn = end;
		}
	};

	Surface& s1 = vec->back();
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			if (x == 0 && y == 0)
				continue;

			auto vec = surfGrid.find(pos + GridVec2(x,y));
			if (vec != surfGrid.end()) {
				//Log::msg(std::to_string(x) +", "+std::to_string(y));

				for (Surface& s2 : vec->second) {
					tryConnect(s1, s2);	
				}

			}
		}
	}

}

void SurfaceMap::removeSurface(Surface ss, GridVec2 pos) {
	Surface sr = Surface(ss.line.reverse());
	GridVec2 p = pos;

	if (ss.line.isHorizontal()) {
		if (ss.line.start.y == pos.y * tileSpacing) {
			p.y--;	
		}
		else if (ss.line.start.y == (pos.y + 1) * tileSpacing) {
			p.y++;	
		}
	}
	else if (ss.line.isVertical()) {
		if (ss.line.start.x == pos.x * tileSpacing) {
			p.x--;	
		}
		else if (ss.line.start.x == (pos.x + 1) * tileSpacing) {
			p.x++;	
		}
	}
	
	addSurface(sr, p);	
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

