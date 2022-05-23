#include "../hpp/game_map.hpp"

#include <random>
#include <time.h>
#include <iostream>

namespace dxe {

	game_map::game_map(int width, int height, int maxFloorCount, int startingMinimunRoomCount, glm::vec2 cellDimension) :
		width(width),
		height(height),
		maxFloorCount(maxFloorCount),
		minRoomCount(startingMinimunRoomCount),
		cellDimension(cellDimension) {

		if (maxFloorCount <= 0 || height <= 0 || width <= 0 || cellDimension.x <= 0.f || cellDimension.y <= 0.f) {
			// error
			//assert(false && "!!!Invalid game map parameters!!!");
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

		//std::srand(time(0));
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
		int i = 0;
		int index = 0;
		int nextFloorId = floorIds[0];
		for (i = 0; i < maxFloorCount; ++i) { // finding a clear floor and the next id number
			if (floorIds[i] > nextFloorId) {
				nextFloorId = floorIds[i];
			}

			if (floorIds[i] == -1) {
				index = i;
			}
		}
		floorIds[index] = ++nextFloorId;

		randomWalkGeneration(map[i]);

	}

	void game_map::printMapData() {
		if (map) {
			std::cout << "Floor Count: " << maxFloorCount << "\n";

			for (int i = 0; i < maxFloorCount; ++i) {
				std::cout << "\n================= Floor: " << floorIds[i] << " =================\n";

				for (int r = 0; r < height; ++r) {
					if (!map[i][r]) {
						std::cout << "INVALID ROW DATA!!!\n";
						continue;
					}
					for (int c = 0; c < width; ++c) {
						if (map[i][r][c].active) {
							std::cout << "X";
						}
						else {
							std::cout << "0";
						}
					} // end for c
					std::cout << "\n";
				} // end for r

			} // end for i

		}
	}

	void game_map::generateDungeon() {
		for (int i = 0; i < maxFloorCount; ++i) {
			randomWalkGeneration(map[i]);
		}
	}
	void game_map::randomWalkGeneration(map_room**& floor) {
		int currX = width >> 1; // half
		int currY = height >> 1;

		int prevX = -1;
		int prevY = -1;

		int roomCount = 0;

		NEIGHBOR currDirection = NEIGHBOR::NONE;
		NEIGHBOR oppositeDirection = NEIGHBOR::NONE;

		if (width * height <= minRoomCount) {
			// error
		}

		while (roomCount < minRoomCount) {
			int direction = std::rand() % 4 + 1;
			// number for 1 to 4
			// 1 = up
			// 2 = down
			// 3 = left
			// 4 = right

			prevX = currX;
			prevY = currY;

			switch (direction)
			{
			case 1:
			{
				currDirection = NEIGHBOR::UP;
				oppositeDirection = NEIGHBOR::DOWN;
				++currY;
				break;
			}
			case 2:
			{
				currDirection = NEIGHBOR::DOWN;
				oppositeDirection = NEIGHBOR::UP;
				--currY;
				break;
			}
			case 3:
			{
				currDirection = NEIGHBOR::LEFT;
				oppositeDirection = NEIGHBOR::RIGHT;
				--currX;
				break;
			}
			default:
			{
				currDirection = NEIGHBOR::RIGHT;
				oppositeDirection = NEIGHBOR::LEFT;
				++currX;
				break;
			}
			}

			// checking if we went out of bounds
			if (currX >= width) {
				--currX;
				continue;
			}

			if (currX < 0) {
				++currX;
				continue;
			}

			if (currY >= height) {
				--currY;
				continue;
			}

			if (currY < 0) {
				++currY;
				continue;
			}


			if (floor[currY][currX].active) { continue; }

			++roomCount;
			floor[currY][currX].active = true;
			floor[currY][currX].addNeighbor(oppositeDirection);

			if (prevX != -1 && prevY != -1) {
				floor[prevY][prevX].addNeighbor(currDirection);
			}
		}

		// you could increase minimum room count here for next levels to be more long or complex
	}
}