#include "../hpp/game_map.hpp"


namespace dxe {

	game_map::game_map(int width, int height, int maxFloorCount, glm::vec2 cellDimension) :
		width(width),
		height(height),
		maxFloorCount(maxFloorCount),
		cellDimension(cellDimension) {

		if (maxFloorCount <= 0 || height <= 0 || width <= 0 || cellDimension.x <= 0.f || cellDimension.y <= 0.f) {
			// error
			assert(false && "!!!Invalid game map parameters!!!");
		}
		map = new map_room * *[maxFloorCount]; // all possible floors

		// allocating floor id array
		floorIds = new int[maxFloorCount];

		int floor, row;
		for (floor = 0; floor < maxFloorCount; ++floor) { // for every floor
			map[floor] = new map_room * [height]; // all possible rows for this floor

			for (row = 0; row < height; ++row) { // for every row in this floor
				map[floor][row] = new map_room[width]; // all possible columns for this floor
			}

			floorIds[floor] = floor + 1; // assigning initial floor ids
		}

		generateDungeon();
	}

	game_map::~game_map() {
		if (map) {
			int floor, row;
			for (floor = 0; floor < maxFloorCount; ++floor) {
				if (map[floor]) {
					for (row = 0; row < height; ++row) {
						if (map[floor][row]) { delete[] map[floor][row]; } // deleting column
					}
					delete[] map[floor]; // deleting row
				}
			}
			delete[] map; // deleting map
		}

		if (floorIds) {
			delete[] floorIds;
		}
	}

	void game_map::clearFloor(int floor) {
		int index = 0;
		int r, c;
		for (r = 0; r < maxFloorCount; ++r) {
			if (floor == floorIds[r]) {
				index = r;
				break;
			}
		}

		for (r = 0; r < height; ++r) {
			for (c = 0; c < width; ++c) {
				map[index][r][c].clear();
			}
		}
		floorIds[index] = -1;
	}

	void game_map::generateNextFloor() {
	}

	void game_map::generateDungeon() {

	}
}