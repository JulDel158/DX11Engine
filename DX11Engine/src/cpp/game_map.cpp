#include "../hpp/game_map.hpp"

#include <random>
#include <time.h>
#include <iostream>

namespace dxe {

	game_map::game_map(uint64_t width, uint64_t height, uint64_t maxFloorCount, uint64_t startingMinimunRoomCount, glm::vec2 cellDimension) :
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
		
		uint64_t floor, row;
		for (floor = 0; floor < maxFloorCount; ++floor) { // for every floor
			map[floor] = new map_room * [height]; // all possible rows for this floor

			for (row = 0; row < height; ++row) { // for every row in this floor
				map[floor][row] = new map_room[width]; // all possible columns for this floor
				
			}

			floorIds[floor] = floor + 1; // assigning initial floor ids
		}

		initializeMidPoints();

		generateDungeon();
	}

	game_map::~game_map() {
		uint64_t floor, row;
		if (map) {
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

		if (midpoints) {
			for (row = 0; row < height; ++row) {
				if (midpoints[row]) { delete[] midpoints[row]; }
			}
			delete[] midpoints;
		}
	}

	void game_map::clearFloor(uint64_t floor) {
		uint64_t index = 0;
		uint64_t r, c;
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

	void game_map::clearSmallestFloor() {
		if (maxFloorCount <= 0) { return; }

		uint64_t index = 0;
		uint64_t minFloor = floorIds[0];
		uint64_t r, c;

		for (r = 0; r < maxFloorCount; ++r) {
			if (floorIds[r] < minFloor) {
				minFloor = floorIds[r];
				index = r;
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
		uint64_t i = 0;
		uint64_t index = 0;
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
		randomWalkGeneration(map[index]);
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

	void game_map::generateRoomMeshes(GameObject*& buffer, uint64_t startPos, uint64_t size) {
		uint64_t col = 0, row = 0, floor = 0, currMesh = startPos;

		if (size - startPos < getRequiredMeshCount()) {
			// ERROR
			return;
		}

		for (floor = 0; floor < maxFloorCount; ++floor) {
			for (row = 0; row < height; ++row) {
				for (col = 0; col < width; ++col) {
					auto& currentRoom = map[floor][row][col];

					currentRoom.roomObjs.resize(MESH_PER_ROOM_COUNT, nullptr);
					glm::vec3 currPos = glm::vec3(midpoints[row][col][3][0], midpoints[row][col][3][1], midpoints[row][col][3][2]);
					for (auto& ptr : currentRoom.roomObjs) {
						ptr = &buffer[currMesh++];
						
						ptr->model.MakeFloorPlane(cellDimension.x, cellDimension.y);
						ptr->setPosition(currPos);
						ptr->resourceId = 7;
						ptr->isActive = currentRoom.active;
					} // assigning allocated gameObjs
						
				}
			}
		}
	}

	uint64_t game_map::getRequiredMeshCount()
	{
		uint64_t count = 0;

		// the total count would be every possible room count, so just multiply the dimmensions of the dungeon with the room count
		
		count = maxFloorCount * width * height * MESH_PER_ROOM_COUNT;

		return count;
	}

	void game_map::generateDungeon() {
		for (int i = 0; i < maxFloorCount; ++i) {
			randomWalkGeneration(map[i]);
		}
	}

	void game_map::randomWalkGeneration(map_room**& floor) {
		int currX = static_cast<int>(width >> 1); // half
		int currY = static_cast<int>(height >> 1);

		int prevX = -1;
		int prevY = -1;

		int roomCount = 0;

		NEIGHBOR currDirection = NEIGHBOR::NONE;
		NEIGHBOR oppositeDirection = NEIGHBOR::NONE;

		if (width * height <= minRoomCount) {
			// error
			return;
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
			floor[currY][currX].activate();
			floor[currY][currX].addNeighbor(oppositeDirection);

			if (prevX != -1 && prevY != -1) {
				floor[prevY][prevX].addNeighbor(currDirection);
			}
		}

		// you could increase minimum room count here for next levels to be more long or complex
	}

	void game_map::initializeMidPoints() {
		int row = 0, col = 0;
		float xpos = 0.f, zpos = 0.f;
		midpoints = new glm::mat4 * [height];
		for (row = 0; row < height; ++row) {
			midpoints[row] = new glm::mat4[width];
		}

		for (row = 0; row < height; ++row) {
			for (col = 0, xpos = 0; col < width; ++col) {
				midpoints[row][col] = glm::mat4{ 1.f };
				midpoints[row][col][3][0] = xpos;
				midpoints[row][col][3][2] = zpos;
				xpos += cellDimension.x;
			}
			zpos += cellDimension.y;
		}
	}
}