#include "../hpp/game_map.hpp"

#include <random>
#include <time.h>
#include <iostream>

#include <glm/gtc/random.hpp>

namespace dxe {

	game_map::game_map(uint64_t width, uint64_t height, uint64_t maxFloorCount, uint64_t startingMinimunRoomCount, glm::vec2 maxCellDimension, glm::vec2 minCellDimension, float offset) :
		gridWidth(width),
		gridHeight(height),
		maxFloorCount(maxFloorCount),
		minRoomCount(startingMinimunRoomCount),
		maxCellDimension(maxCellDimension),
		minCellDimension(minCellDimension),
		roomOffset(offset){

		if (maxFloorCount <= 0 || height <= 0 || width <= 0 || maxCellDimension.x <= 0.f || maxCellDimension.y <= 0.f || minCellDimension.x <= 0.f || minCellDimension.y <= 0.f ||
			maxCellDimension.x < minCellDimension.x || maxCellDimension.y < minCellDimension.y) {
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

			floorIds[floor] = (int)floor + 1; // assigning initial floor ids
		}
		// 0 to height - 1 = all hallways going across the x axis
		// from height to width - 1 = all hallways going across the z axis
		hallways = new GameObject * [width + height]{ nullptr };

		initializeMidPoints();

		//generateDungeon();
	}

	game_map::~game_map() {
		uint64_t floor, row;
		if (map) {
			for (floor = 0; floor < maxFloorCount; ++floor) {
				if (map[floor]) {
					for (row = 0; row < gridHeight; ++row) {
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
			for (row = 0; row < gridHeight; ++row) {
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

		for (r = 0; r < gridHeight; ++r) {
			for (c = 0; c < gridWidth; ++c) {
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

		for (r = 0; r < gridHeight; ++r) {
			for (c = 0; c < gridWidth; ++c) {
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

				for (int r = 0; r < gridHeight; ++r) {
					if (!map[i][r]) {
						std::cout << "INVALID ROW DATA!!!\n";
						continue;
					}
					for (int c = 0; c < gridWidth; ++c) {
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
			for (row = 0; row < gridHeight; ++row) {
				for (col = 0; col < gridWidth; ++col) {
					auto& currentRoom = map[floor][row][col];

					currentRoom.roomObjs.resize(MESH_PER_ROOM_COUNT, nullptr);
					glm::vec3 currPos = glm::vec3(midpoints[row][col][3][0], midpoints[row][col][3][1], midpoints[row][col][3][2]);
					for (auto& ptr : currentRoom.roomObjs) {
						ptr = &buffer[currMesh++];
						
						// most of this code should be move to room activate function
						/*ptr->model.MakeFloorPlane(maxCellDimension.x, maxCellDimension.y);
						ptr->setPosition(currPos);
						ptr->resourceId = 7;
						ptr->isActive = currentRoom.active;*/
					} // assigning allocated gameObjs	
				}
			}
		}

		for (floor = 0; floor < gridWidth + gridHeight; ++floor) {
			hallways[floor] = &buffer[currMesh++];
		}

		generateHallwayMeshes();
	}

	uint64_t game_map::getRequiredMeshCount()
	{
		uint64_t count = 0;

		// the total count is every possible room count, so just multiply the dimmensions of the dungeon with the amount of meshes needed per room
		// + every hallway mesh that needs to be made (1 for each row and column)
		
		count = (maxFloorCount * gridWidth * gridHeight * MESH_PER_ROOM_COUNT) + gridWidth + gridHeight;

		return count;
	}

	void game_map::generateDungeon() {
		for (int i = 0; i < maxFloorCount; ++i) {
			randomWalkGeneration(map[i]);
		}
	}

	glm::vec3 game_map::getRandomActiveRoomPos()
	{
		if (!map) {
			return glm::vec3(0.f);
		}
		uint64_t row, col;
		
		for (row = 0; row < gridHeight; ++row) {
			for (col = 0; col < gridWidth; ++col) {
				if (map[0][row][col].active) {
					return glm::vec3(midpoints[row][col][3][0], midpoints[row][col][3][1], midpoints[row][col][3][2]);
				}
			}
		}

		return glm::vec3(0.f);
	}

	void game_map::randomWalkGeneration(map_room**& floor) {
		int currX = static_cast<int>(gridWidth >> 1); // half
		int currY = static_cast<int>(gridHeight >> 1);

		int prevX = -1;
		int prevY = -1;

		int roomCount = 0;

		NEIGHBOR currDirection = NEIGHBOR::NONE;
		NEIGHBOR oppositeDirection = NEIGHBOR::NONE;

		glm::vec3 currPos{ 0.f };

		if (gridWidth * gridHeight <= minRoomCount) {
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
			if (currX >= gridWidth) {
				--currX;
				continue;
			}

			if (currX < 0) {
				++currX;
				continue;
			}

			if (currY >= gridHeight) {
				--currY;
				continue;
			}

			if (currY < 0) {
				++currY;
				continue;
			}


			if (floor[currY][currX].active) { continue; }

			currPos = glm::vec3(midpoints[currY][currX][3][0], midpoints[currY][currX][3][1], midpoints[currY][currX][3][2]);

			++roomCount;
			floor[currY][currX].activate(minCellDimension, maxCellDimension, currPos);
			floor[currY][currX].addNeighbor(oppositeDirection);

			if (prevX != -1 && prevY != -1 && (prevX != currX || prevY != currY)) {
				floor[prevY][prevX].addNeighbor(currDirection);
			}
		}

		// you could increase minimum room count here for next levels to be more long or complex
	}

	void game_map::initializeMidPoints() {
		int row = 0, col = 0;
		float xpos = 0.f, zpos = 0.f;
		midpoints = new glm::mat4 * [gridHeight];
		for (row = 0; row < gridHeight; ++row) {
			midpoints[row] = new glm::mat4[gridWidth];
		}

		for (row = 0; row < gridHeight; ++row) {
			for (col = 0, xpos = 0; col < gridWidth; ++col) {
				midpoints[row][col] = glm::mat4{ 1.f };
				midpoints[row][col][3][0] = xpos;
				midpoints[row][col][3][2] = zpos;
				xpos += maxCellDimension.x + roomOffset;
			}
			zpos += maxCellDimension.y + roomOffset;
		}
	}

	void game_map::generateHallwayMeshes() {
		// TODO: create the static mesh for the hallways

		Objectdata xAxisHall, zAxisHall;

		glm::vec3 xCenter{ 0.f }, zCenter{ 0.f };
		float xLenght{ 0.f }, zLenght{ 0.f };
		float cWidth = static_cast<float>(gridWidth);
		float cHeight = static_cast<float>(gridHeight);

		uint64_t col = 0, row = 0;

		for (; row < gridHeight; ++row) {
			zCenter += glm::vec3(midpoints[row][0][3][0], midpoints[row][0][3][1] - 0.25f, midpoints[row][0][3][2]);
		}
		zCenter /= cHeight;

		for (; col < gridWidth; ++col) {
			xCenter += glm::vec3(midpoints[0][col][3][0], midpoints[0][col][3][1] - 0.25f, midpoints[0][col][3][2]);
		}
		xCenter /= cWidth;

		xLenght = (maxCellDimension.x * cWidth) + roomOffset * (cWidth - 1.f);
		zLenght = (maxCellDimension.y * cHeight) + roomOffset * (cHeight - 1.f);

		xAxisHall.MakeFloorPlane(xLenght, roomOffset);
		zAxisHall.MakeFloorPlane(roomOffset, zLenght);

		// 0 to height - 1 = all hallways going across the x axis
		// from height to width - 1 = all hallways going across the z axis

		for (row = 0; row < gridHeight; ++row) {
			auto& h = hallways[row];
			h->model = xAxisHall;
			h->isActive = true;
			xCenter.z += maxCellDimension.y + roomOffset;
			h->setPosition(xCenter);
		}

		for (col = gridHeight; col < gridHeight + gridWidth; ++col) {
			auto& h = hallways[col];
			h->model = zAxisHall;
			h->isActive = true;
			zCenter.x += maxCellDimension.x + roomOffset;
			h->setPosition(zCenter);
		}
	}

	void map_room::clear() {
		active = false; 
		type = ROOM_TYPE::DEBUG;

		for (auto& n : neightbors) {
			n = NEIGHBOR::NONE;
		}

		if (roomObjs.empty() || !roomObjs.front()) {
			return;
		}

		auto& ptr = roomObjs.front();
		ptr->isActive = false;
	}

	void map_room::addNeighbor(NEIGHBOR n) {
		for (auto& m : neightbors) { 
			if (m == NEIGHBOR::NONE || m == n) { 
				m = n; 
				return; 
			} 
		}
	}

	void map_room::activate(glm::vec2 minSize, glm::vec2 maxSize, glm::vec3 position) {
		active = true;
		roomSize = glm::linearRand(minSize, maxSize);

		if (roomObjs.empty() || !roomObjs.front()) {
			return;
		}
		auto& ptr = roomObjs.front();
		ptr->model.MakeFloorPlane(roomSize.x, roomSize.y);
		ptr->setPosition(position);
		ptr->resourceId = 7;
		ptr->isActive = true;
	}
}