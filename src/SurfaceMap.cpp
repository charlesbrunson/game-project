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

	static auto hasCorner = [](Surface& a, Surface& b) -> Corner {
		Corner r;
		if (a.line.isVertical() || b.line.isVertical())
			return r;
		
		if ((a.line.getVector().x > 0.f) != (b.line.getVector().x > 0.f)) {
			Surface *beforeCorner = nullptr;
			Surface *afterCorner = nullptr;
			if (a.line.end == b.line.start) {
				beforeCorner = &a;
				afterCorner = &b;
			}
			else if (a.line.start == b.line.end) {
				beforeCorner = &b;
				afterCorner = &a;
			}

			if (beforeCorner == nullptr || afterCorner == nullptr)
				return r;

			float angDiff = Math::angle(afterCorner->line.getVector()) - Math::angle(beforeCorner->line.getVector());
			angDiff = Math::normalAng(angDiff);
			angDiff = ceilf(angDiff);

			if (angDiff > 0.f && angDiff < PI_F) {
				r.valid = true;
				r.position = beforeCorner->line.end;
				r.normal = Vec2(beforeCorner->line.getVector().x > 0.f ? 1.f : -1.f, 0.f);
			}
		}
		return r;
	};

	static auto addCorner = [](Corner& corner, GridVec2 pos, GridMap<std::vector<Corner>>* cGrid) -> bool {
		bool addCorner = false;
		if (corner.valid) {
			addCorner = true;
			//try add corner
			auto cp = cGrid->find(pos);
			if (cp == cGrid->end()) {
				cp = cGrid->insert(std::make_pair(pos, std::vector<Corner>())).first;
				cp->second.push_back(corner);
			}
			else {
				for (auto c = cp->second.begin(); c != cp->second.end(); c++) {
					if (c->position == corner.position) {
						if (c->normal == corner.normal) {
							addCorner = false;
							break;
						}
						else if (c->normal == -corner.normal) {
							addCorner = false;
							cp->second.erase(c);
							break;
						}

					}

				}
				if (addCorner)
					cp->second.push_back(corner);
			}
		}
		return addCorner;
	};

	Surface& s1 = vec->back();
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {

			auto vec = surfGrid.find(pos + GridVec2(x,y));
			if (vec != surfGrid.end()) {

				for (Surface& s2 : vec->second) {
					if (s1.line != s2.line) {
						tryConnect(s1, s2);	
						Corner corner = hasCorner(s1, s2);

						GridVec2 cornerPos( floorf(corner.position.x / tileSpacing), floorf(corner.position.y / tileSpacing) );

						addCorner(corner, cornerPos, &cornerGrid);	
					}
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
std::vector<Corner>* SurfaceMap::getCornersInGrid(GridVec2 place) {
	return cornerGrid.find(place) != cornerGrid.end() ? &cornerGrid.at(place) : nullptr;	
}

GridMap<std::vector<Surface>>* SurfaceMap::getAllSurfaces() {
	return &surfGrid;
}
GridMap<std::vector<Corner>>* SurfaceMap::getAllCorners() {
	return &cornerGrid;
}

/*
std::vector<Surface*> SurfaceMap::surfacesStartingAt(Point p) {

}

std::vector<Surface*> SurfaceMap::surfacesEndingAt(Point p) {

}

void SurfaceMap::initGrid() {

}
*/

