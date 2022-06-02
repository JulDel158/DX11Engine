#include "../hpp/game_map.hpp"

#include <random>
#include <time.h>
#include <iostream>

#include <glm/gtc/random.hpp>

namespace dxe {

	game_map::game_map(uint64_t width, uint64_t height, uint64_t maxFloorCount, uint64_t startingMinimunRoomCount, glm::vec2 maxCellDimension, glm::vec2 minCellDimension, float offset, float hallWidth, float hallHeight) :
		gridWidth(width),
		gridHeight(height),
		maxFloorCount(maxFloorCount),
		minRoomCount(startingMinimunRoomCount),
		maxCellDimension(maxCellDimension),
		minCellDimension(minCellDimension),
		roomOffset(offset),
		hallwayWidth(hallWidth),
		hallwayHeight(hallHeight){

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
		//hallways = new GameObject * [width + height]{ nullptr };
		//hallway = new GameObject* { nullptr };

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

		/*if (hallways) {
			delete[] hallways;
		}*/

		/*if (hallway) {
			delete hallway;
		}*/
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
		uint64_t /*col = 0, row = 0, floor = 0,*/ currMesh = startPos;

		if (size - startPos < getRequiredMeshCount()) {
			// ERROR
			return;
		}

		//for (floor = 0; floor < maxFloorCount; ++floor) {
		//	for (row = 0; row < gridHeight; ++row) {
		//		for (col = 0; col < gridWidth; ++col) {
		//			auto& currentRoom = map[floor][row][col];

		//			currentRoom.roomObjs.resize(MESH_PER_ROOM_COUNT, nullptr);
		//			glm::vec3 currPos = glm::vec3(midpoints[row][col][3][0], midpoints[row][col][3][1], midpoints[row][col][3][2]);
		//			for (auto& ptr : currentRoom.roomObjs) {
		//				ptr = &buffer[currMesh++];
		//				
		//				// most of this code should be move to room activate function
		//				/*ptr->model.MakeFloorPlane(maxCellDimension.x, maxCellDimension.y);
		//				ptr->setPosition(currPos);
		//				ptr->resourceId = 7;
		//				ptr->isActive = currentRoom.active;*/
		//			} // assigning allocated gameObjs	
		//		}
		//	}
		//}

		/*for (floor = 0; floor < gridWidth + gridHeight; ++floor) {
			hallways[floor] = &buffer[currMesh++];
		}*/
		roomObj = &buffer[currMesh++];
		hallwayObj = &buffer[currMesh++];

		//generateHallwayMeshes();
		//generateHallwayMesh();
	}

	uint64_t game_map::getRequiredMeshCount()
	{
		uint64_t count = 2;

		// the total count is every possible room count, so just multiply the dimmensions of the dungeon with the amount of meshes needed per room
		// + every hallway mesh that needs to be made (1 for each row and column)
		
		//count = (maxFloorCount * gridWidth * gridHeight * MESH_PER_ROOM_COUNT) + 1;
		return count;
	}

	void game_map::generateDungeon() {
		for (int i = 0; i < maxFloorCount; ++i) {
			randomWalkGeneration(map[i]);
		}

		generateRoomMesh();
		generateHallwayMesh();
	}

	void game_map::generateDebugDungeon() {
		for (int row = 0; row < gridHeight; ++row) {
			for (int col = 0; col < gridWidth; ++col) {
				auto currPos = glm::vec3(midpoints[row][col][3][0], midpoints[row][col][3][1], midpoints[row][col][3][2]);
				map[0][row][col].activate(maxCellDimension, minCellDimension, currPos);

				if (row != 0) {
					map[0][row][col].addNeighbor(NEIGHBOR::DOWN);
				}

				if (row != gridHeight - 1) {
					map[0][row][col].addNeighbor(NEIGHBOR::UP);
				}

				if (col != 0) {
					map[0][row][col].addNeighbor(NEIGHBOR::LEFT);
				}

				if (col != gridWidth - 1) {
					map[0][row][col].addNeighbor(NEIGHBOR::RIGHT);
				}
			}
		}

		generateRoomMesh();
		generateHallwayMesh();
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

		bool firstIteration = true;
		while (roomCount < minRoomCount) {

			// BUG: index values going far out of scope

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
			if (currX >= static_cast<int>(gridWidth)) {
				currX = static_cast<int>(gridWidth) - 1;
				//continue;
			}

			if (currX < 0) {
				currX = 0;
				//continue;
			}

			if (currY >= static_cast<int>(gridHeight)) {
				currY = static_cast<int>(gridHeight) - 1;
				//continue;
			}

			if (currY < 0) {
				currY = 0;
				//continue;
			}


			if (floor[currY][currX].active) { continue; }

			currPos = glm::vec3(midpoints[currY][currX][3][0], midpoints[currY][currX][3][1], midpoints[currY][currX][3][2]);

			++roomCount;
			floor[currY][currX].activate(minCellDimension, maxCellDimension, currPos);
			

			if (!firstIteration && (prevX != currX || prevY != currY)) {
				floor[currY][currX].addNeighbor(oppositeDirection);
				floor[prevY][prevX].addNeighbor(currDirection);
			}

			firstIteration = false;
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
		//// TODO: create the static mesh for the hallways

		//Objectdata xAxisHall, zAxisHall;

		//glm::vec3 xCenter{ 0.f }, zCenter{ 0.f };
		//float xLenght{ 0.f }, zLenght{ 0.f };
		//float cWidth = static_cast<float>(gridWidth);
		//float cHeight = static_cast<float>(gridHeight);

		//uint64_t col = 0, row = 0;

		//for (; row < gridHeight; ++row) {
		//	zCenter += glm::vec3(midpoints[row][0][3][0], midpoints[row][0][3][1] - 0.25f, midpoints[row][0][3][2]);
		//}
		//zCenter /= cHeight;

		//for (; col < gridWidth; ++col) {
		//	xCenter += glm::vec3(midpoints[0][col][3][0], midpoints[0][col][3][1] - 0.25f, midpoints[0][col][3][2]);
		//}
		//xCenter /= cWidth;

		//xLenght = (maxCellDimension.x * cWidth) + roomOffset * (cWidth - 1.f);
		//zLenght = (maxCellDimension.y * cHeight) + roomOffset * (cHeight - 1.f);

		//xAxisHall.MakeFloorPlane(xLenght, roomOffset);
		//zAxisHall.MakeFloorPlane(roomOffset, zLenght);

		//// 0 to height - 1 = all hallways going across the x axis
		//// from height to width - 1 = all hallways going across the z axis

		//for (row = 0; row < gridHeight; ++row) {
		//	auto& h = hallways[row];
		//	h->model = xAxisHall;
		//	h->isActive = true;
		//	h->setPosition(xCenter);
		//	h->resourceId = 3;
		//	xCenter.z += maxCellDimension.y + roomOffset;
		//}

		//for (col = gridHeight; col < gridHeight + gridWidth; ++col) {
		//	auto& h = hallways[col];
		//	h->model = zAxisHall;
		//	h->isActive = true;
		//	h->setPosition(zCenter);
		//	h->resourceId = 2;
		//	zCenter.x += maxCellDimension.x + roomOffset;
		//}
	}

	void game_map::generateHallwayMesh() {
		// we will traverse every cell and check if we can build a neightbor to the left and up
		hallwayObj->model.vertices.clear();
		hallwayObj->model.indices.clear();
		hallwayObj->isActive = true;
		hallwayObj->resourceId = 4;

		// we can reserve the vertices and indices to the max possible capacity

		int row = 0, col = 0;
		bool hasUp = false, hasRight = false;

		for (row = 0; row < static_cast<int>(gridHeight); ++row) {
			for (col = 0; col < static_cast<int>(gridWidth); ++col) {
				if (!map[0][row][col].active) {
					continue;
				}
				hasUp = false;
				hasRight = false;

				// now we check if the current room has a neighbor to it's right or up in the grid
				for (int i = 0; i < 4; ++i ) {
					if (map[0][row][col].neightbors[i] == NEIGHBOR::RIGHT) {
						hasRight = true;
					} else if (map[0][row][col].neightbors[i] == NEIGHBOR::UP) {
						hasUp = true;
					}
				}


				// now we contruct each hallway

				if (hasRight) { // x axis hall
					glm::vec3 startPos = GetPositionVector(midpoints[row][col]);
					startPos.x += map[0][row][col].roomSize.x / 2.f;
					glm::vec3 endPos = GetPositionVector(midpoints[row][col + 1]);
					endPos.x -= map[0][row][col + 1].roomSize.x / 2.f;



					/*
					   p2/6    p3/7
						*       *
						
					  sp*      en*

					   p1/5    p4/8
						*       * 
						
					*/
					ObjVertex p1;
					ObjVertex p2;
					ObjVertex p3;
					ObjVertex p4;

					ObjVertex p5;
					ObjVertex p6;
					ObjVertex p7;
					ObjVertex p8;

					/*p1.uv = p5.uv = { 0.f, 0.f };
					p2.uv = p6.uv = { 0.f, 1.f };

					p3.uv = p7.uv = { 1.f, 1.f };
					p4.uv = p8.uv = { 1.f, 0.f };*/

					p1.pos = p2.pos = p5.pos = p6.pos = startPos;
					p3.pos = p4.pos = p7.pos = p8.pos = endPos;

					p1.pos.z -= hallwayWidth / 2.f;
					p2.pos.z += hallwayWidth / 2.f;

					p5.pos.z = p4.pos.z = p8.pos.z = p1.pos.z; 
					p6.pos.z = p3.pos.z = p7.pos.z = p2.pos.z;

					p5.pos.y = p6.pos.y = p7.pos.y = p8.pos.y = hallwayHeight;

					// NOW WE CAN PUSH THESE POINTS
					const int currentIndx = hallwayObj->model.vertices.size(); // basically our 0 index

					hallwayObj->model.vertices.push_back(p1);
					hallwayObj->model.vertices.push_back(p2);
					hallwayObj->model.vertices.push_back(p3);
					hallwayObj->model.vertices.push_back(p4);
					hallwayObj->model.vertices.push_back(p5);
					hallwayObj->model.vertices.push_back(p6);
					hallwayObj->model.vertices.push_back(p7);
					hallwayObj->model.vertices.push_back(p8);

					//floor 
					hallwayObj->model.indices.push_back(currentIndx);
					hallwayObj->model.indices.push_back(currentIndx + 1);
					hallwayObj->model.indices.push_back(currentIndx + 2);

					hallwayObj->model.indices.push_back(currentIndx);
					hallwayObj->model.indices.push_back(currentIndx + 2);
					hallwayObj->model.indices.push_back(currentIndx + 3);

					// wall 1
					hallwayObj->model.indices.push_back(currentIndx);
					hallwayObj->model.indices.push_back(currentIndx + 4);
					hallwayObj->model.indices.push_back(currentIndx + 7);

					hallwayObj->model.indices.push_back(currentIndx);
					hallwayObj->model.indices.push_back(currentIndx + 7);
					hallwayObj->model.indices.push_back(currentIndx + 3);

					// wall 2
					hallwayObj->model.indices.push_back(currentIndx + 1);
					hallwayObj->model.indices.push_back(currentIndx + 5);
					hallwayObj->model.indices.push_back(currentIndx + 6);

					hallwayObj->model.indices.push_back(currentIndx + 1);
					hallwayObj->model.indices.push_back(currentIndx + 6);
					hallwayObj->model.indices.push_back(currentIndx + 2);

					// may be removed
					// ceiling 
					hallwayObj->model.indices.push_back(currentIndx + 4);
					hallwayObj->model.indices.push_back(currentIndx + 6);
					hallwayObj->model.indices.push_back(currentIndx + 5);

					hallwayObj->model.indices.push_back(currentIndx + 4);
					hallwayObj->model.indices.push_back(currentIndx + 7);
					hallwayObj->model.indices.push_back(currentIndx + 6);
				}


				if (hasUp) { // z axis hall
					glm::vec3 startPos = GetPositionVector(midpoints[row][col]);
					startPos.z += map[0][row][col].roomSize.y / 2.f;
					glm::vec3 endPos = GetPositionVector(midpoints[row + 1][col]);
					endPos.z -= map[0][row + 1][col].roomSize.y / 2.f;

					/*
					   p2/6   en*  p3/7
						*           *

					       

					   p1/5  sp*  p4/8
						*           *

					*/
					ObjVertex p1;
					ObjVertex p2;
					ObjVertex p3;
					ObjVertex p4;

					ObjVertex p5;
					ObjVertex p6;
					ObjVertex p7;
					ObjVertex p8;

					/*p1.uv = p5.uv = { 0.f, 0.f };
					p2.uv = p6.uv = { 0.f, 1.f };

					p3.uv = p7.uv = { 1.f, 1.f };
					p4.uv = p8.uv = { 1.f, 0.f };*/

					p1.pos = p4.pos = p5.pos = p8.pos = startPos;
					p2.pos = p3.pos = p6.pos = p7.pos = endPos;

					p1.pos.x -= hallwayWidth / 2.f; // TODO: REPLACE FOR ROOM WIDTH
					p4.pos.x += hallwayWidth / 2.f;

					p5.pos.x = p2.pos.x = p6.pos.x = p1.pos.x;
					p8.pos.x = p3.pos.x = p7.pos.x = p4.pos.x;

					p5.pos.y = p6.pos.y = p7.pos.y = p8.pos.y = hallwayHeight;

					// NOW WE CAN PUSH THESE POINTS
					const int currentIndx = hallwayObj->model.vertices.size(); // basically our 0 index

					hallwayObj->model.vertices.push_back(p1);
					hallwayObj->model.vertices.push_back(p2);
					hallwayObj->model.vertices.push_back(p3);
					hallwayObj->model.vertices.push_back(p4);
					hallwayObj->model.vertices.push_back(p5);
					hallwayObj->model.vertices.push_back(p6);
					hallwayObj->model.vertices.push_back(p7);
					hallwayObj->model.vertices.push_back(p8);

					//floor 
					hallwayObj->model.indices.push_back(currentIndx);
					hallwayObj->model.indices.push_back(currentIndx + 1);
					hallwayObj->model.indices.push_back(currentIndx + 2);

					hallwayObj->model.indices.push_back(currentIndx);
					hallwayObj->model.indices.push_back(currentIndx + 2);
					hallwayObj->model.indices.push_back(currentIndx + 3);

					// wall 1
					hallwayObj->model.indices.push_back(currentIndx);
					hallwayObj->model.indices.push_back(currentIndx + 4);
					hallwayObj->model.indices.push_back(currentIndx + 5);

					hallwayObj->model.indices.push_back(currentIndx);
					hallwayObj->model.indices.push_back(currentIndx + 5);
					hallwayObj->model.indices.push_back(currentIndx + 1);

					// wall 2
					hallwayObj->model.indices.push_back(currentIndx + 3);
					hallwayObj->model.indices.push_back(currentIndx + 7);
					hallwayObj->model.indices.push_back(currentIndx + 6);

					hallwayObj->model.indices.push_back(currentIndx + 3);
					hallwayObj->model.indices.push_back(currentIndx + 6);
					hallwayObj->model.indices.push_back(currentIndx + 2);

					// may be removed
					// ceiling 
					hallwayObj->model.indices.push_back(currentIndx + 4);
					hallwayObj->model.indices.push_back(currentIndx + 6);
					hallwayObj->model.indices.push_back(currentIndx + 5);

					hallwayObj->model.indices.push_back(currentIndx + 4);
					hallwayObj->model.indices.push_back(currentIndx + 7);
					hallwayObj->model.indices.push_back(currentIndx + 6);
				}


			}
		}
		int debug = 0;
	}

	void game_map::generateRoomMesh() {
		// much like the hallway generation, we will traverse to every active room
		// and for that room we will add the floor and wall geometry into the room obj
		roomObj->model.vertices.clear();
		roomObj->model.indices.clear();
		roomObj->isActive = true;
		roomObj->resourceId = 7;

		int row = 0, col = 0;
		bool hasLeft = false, hasRight = false, hasUp = false, hasDown = false; // theses will be used to determine wheter a door or a wall should be placed

		for (row = 0; row < static_cast<int>(gridHeight); ++row) {
			for (col = 0; col < static_cast<int>(gridWidth); ++col) {

				if (!map[0][row][col].active) { continue; }
				hasLeft = false;
				hasRight = false;
				hasUp = false;
				hasDown = false;

				// now we check if the current room has a neighbor to it's right or up in the grid
				for (int i = 0; i < 4; ++i) {
					if (map[0][row][col].neightbors[i] == NEIGHBOR::RIGHT) {
						hasRight = true;
					}
					else if (map[0][row][col].neightbors[i] == NEIGHBOR::UP) {
						hasUp = true;
					}
					else if (map[0][row][col].neightbors[i] == NEIGHBOR::LEFT) {
						hasLeft = true;
					}
					else if (map[0][row][col].neightbors[i] == NEIGHBOR::DOWN) {
						hasDown = true;
					}
				}
				
				glm::vec3 center = GetPositionVector(midpoints[row][col]);

				// we will need the main 8 vertices
				// and the we will need 4 more vertices that will be used for doors

				int currentIndx = roomObj->model.vertices.size();
				// Floor
				ObjVertex p1;
				ObjVertex p2;
				ObjVertex p3;
				ObjVertex p4;

				/*
				p2      p3
				  *     *
				
				  *     *
				p1      p4
				*/

				p1.pos = p2.pos = p3.pos = p4.pos = center;
				p1.nrm = p2.nrm = p3.nrm = p4.nrm = { 0.f, 1.f, 0.f };

				p1.pos.x = p2.pos.x = center.x - map[0][row][col].roomSize.x / 2.f;
				p3.pos.x = p4.pos.x = center.x + map[0][row][col].roomSize.x / 2.f;

				p1.pos.z = p4.pos.z = center.z - map[0][row][col].roomSize.y / 2.f;
				p2.pos.z = p3.pos.z = center.z + map[0][row][col].roomSize.y / 2.f;


				// TODO: ADD SYSTEM AND FUNCTION THAT RETURNS UV COORDINATES TO REPRESENT DIFFERENT TILE FROM AN IMAGE ADDING VARIETY OF FLOOR TEXTURES
				p1.uv = { 0.f, 0.f };
				p2.uv = { 0.f, 1.f };
				p3.uv = { 1.f, 1.f };
				p4.uv = { 1.f, 0.f };


				roomObj->model.vertices.push_back(p1);
				roomObj->model.vertices.push_back(p2);
				roomObj->model.vertices.push_back(p3);
				roomObj->model.vertices.push_back(p4);

				roomObj->model.indices.push_back(currentIndx + 0);
				roomObj->model.indices.push_back(currentIndx + 1);
				roomObj->model.indices.push_back(currentIndx + 2);

				roomObj->model.indices.push_back(currentIndx + 0);
				roomObj->model.indices.push_back(currentIndx + 2);
				roomObj->model.indices.push_back(currentIndx + 3);


				// ceiling
				p1.pos.y = p2.pos.y = p3.pos.y = p4.pos.y = hallwayHeight * 1.5f;
				p1.nrm = p2.nrm = p3.nrm = p4.nrm = { 0.f, -1.f, 0.f };
				// TODO: CHANGE UVS HERE 
				currentIndx = roomObj->model.vertices.size();

				roomObj->model.vertices.push_back(p1);
				roomObj->model.vertices.push_back(p2);
				roomObj->model.vertices.push_back(p3);
				roomObj->model.vertices.push_back(p4);

				roomObj->model.indices.push_back(currentIndx + 0);
				roomObj->model.indices.push_back(currentIndx + 1);
				roomObj->model.indices.push_back(currentIndx + 2);

				roomObj->model.indices.push_back(currentIndx + 0);
				roomObj->model.indices.push_back(currentIndx + 2);
				roomObj->model.indices.push_back(currentIndx + 3);


				// x-axis walls/doors
				ObjVertex p5, p6, p7, p8;
				p1.pos = p2.pos = p3.pos = p4.pos = p5.pos = p6.pos = p7.pos = p8.pos = center;

				// we will calculate all the points and when it is time to add them, we will just check wheter it is a door or not
				// to determine which vertices and indices to add
				// all these points share the same x value but have varying y and z values

				// first we will calculate all the z values
				p1.pos.z = p2.pos.z = center.z - map[0][row][col].roomSize.y / 2.f;
				p3.pos.z = p4.pos.z = center.z + map[0][row][col].roomSize.y / 2.f;

				// the next 4 points should align with the hallway
				p5.pos.z = p6.pos.z = center.z - hallwayWidth / 2.f;
				p7.pos.z = p8.pos.z = center.z + hallwayWidth / 2.f;

				// Now we calculate the y values
				p1.pos.y = p5.pos.y = p8.pos.y = p4.pos.y = 0.f;
				p2.pos.y = p3.pos.y = hallwayHeight * 1.5f;
				p6.pos.y = p7.pos.y = hallwayHeight;

				// now that all the values are where they need to be we can choose which wall we want to make, adjust the x or z values of all
				// the points and build the wall, then repeat


				//Left wall
				p1.pos.x = p2.pos.x = p3.pos.x = p4.pos.x = p5.pos.x = p6.pos.x = p7.pos.x = p8.pos.x = center.x - map[0][row][col].roomSize.x / 2.f;
				// adjusting normals
				p1.nrm = p2.nrm = p3.nrm = p4.nrm = p5.nrm = p6.nrm = p7.nrm = p8.nrm = { 1.f, 0.f, 0.f };
				// TODO CHANGE UV USING FUNCTION WHEN IMPLEMENTED
				p1.uv = { 0.f, 0.f };
				p2.uv = { 0.f, 1.f };
				p3.uv = { 1.f, 1.f };
				p4.uv = { 1.f, 0.f };

				p5.uv = { 0.4f, 0.f };
				p6.uv = { 0.4f, 0.6f };
				p7.uv = { 0.6f, 0.6f };
				p8.uv = { 0.6f, 0.0f };

				currentIndx = roomObj->model.vertices.size();
				if (hasLeft) {
					roomObj->model.vertices.push_back(p1);
					roomObj->model.vertices.push_back(p2);
					roomObj->model.vertices.push_back(p3);
					roomObj->model.vertices.push_back(p4);

					roomObj->model.vertices.push_back(p5);
					roomObj->model.vertices.push_back(p6);
					roomObj->model.vertices.push_back(p7);
					roomObj->model.vertices.push_back(p8);

					// triangle 1
					roomObj->model.indices.push_back(currentIndx + 0);
					roomObj->model.indices.push_back(currentIndx + 1);
					roomObj->model.indices.push_back(currentIndx + 4);

					// triangle 2
					roomObj->model.indices.push_back(currentIndx + 4);
					roomObj->model.indices.push_back(currentIndx + 1);
					roomObj->model.indices.push_back(currentIndx + 5);

					// triangle 3
					roomObj->model.indices.push_back(currentIndx + 1);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 5);

					// triangle 4
					roomObj->model.indices.push_back(currentIndx + 5);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 6);

					// triangle 5
					roomObj->model.indices.push_back(currentIndx + 6);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 7);

					// triangle 6
					roomObj->model.indices.push_back(currentIndx + 7);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 3);
				} 
				else {
					roomObj->model.vertices.push_back(p1);
					roomObj->model.vertices.push_back(p2);
					roomObj->model.vertices.push_back(p3);
					roomObj->model.vertices.push_back(p4);

					// triangle 1
					roomObj->model.indices.push_back(currentIndx + 0);
					roomObj->model.indices.push_back(currentIndx + 1);
					roomObj->model.indices.push_back(currentIndx + 2);

					// triangle 2
					roomObj->model.indices.push_back(currentIndx + 0);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 3);
				}

				//Right wall
				p1.pos.x = p2.pos.x = p3.pos.x = p4.pos.x = p5.pos.x = p6.pos.x = p7.pos.x = p8.pos.x = center.x + map[0][row][col].roomSize.x / 2.f;
				// adjusting normals
				p1.nrm = p2.nrm = p3.nrm = p4.nrm = p5.nrm = p6.nrm = p7.nrm = p8.nrm = { -1.f, 0.f, 0.f };
				currentIndx = roomObj->model.vertices.size();
				if (hasRight) {
					roomObj->model.vertices.push_back(p1);
					roomObj->model.vertices.push_back(p2);
					roomObj->model.vertices.push_back(p3);
					roomObj->model.vertices.push_back(p4);

					roomObj->model.vertices.push_back(p5);
					roomObj->model.vertices.push_back(p6);
					roomObj->model.vertices.push_back(p7);
					roomObj->model.vertices.push_back(p8);

					// triangle 1
					roomObj->model.indices.push_back(currentIndx + 0);
					roomObj->model.indices.push_back(currentIndx + 1);
					roomObj->model.indices.push_back(currentIndx + 4);

					// triangle 2
					roomObj->model.indices.push_back(currentIndx + 4);
					roomObj->model.indices.push_back(currentIndx + 1);
					roomObj->model.indices.push_back(currentIndx + 5);

					// triangle 3
					roomObj->model.indices.push_back(currentIndx + 1);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 5);

					// triangle 4
					roomObj->model.indices.push_back(currentIndx + 5);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 6);

					// triangle 5
					roomObj->model.indices.push_back(currentIndx + 6);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 7);

					// triangle 6
					roomObj->model.indices.push_back(currentIndx + 7);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 3);
				}
				else {
					roomObj->model.vertices.push_back(p1);
					roomObj->model.vertices.push_back(p2);
					roomObj->model.vertices.push_back(p3);
					roomObj->model.vertices.push_back(p4);

					// triangle 1
					roomObj->model.indices.push_back(currentIndx + 0);
					roomObj->model.indices.push_back(currentIndx + 1);
					roomObj->model.indices.push_back(currentIndx + 2);

					// triangle 2
					roomObj->model.indices.push_back(currentIndx + 0);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 3);
				}

				//Up wall

				// reset all points to the center
				p1.pos = p2.pos = p3.pos = p4.pos = p5.pos = p6.pos = p7.pos = p8.pos = center;

				// all these points share the same z value but have varying y and x values

				// first we will calculate all the x values
				p1.pos.x = p2.pos.x = center.x - map[0][row][col].roomSize.x / 2.f;
				p3.pos.x = p4.pos.x = center.x + map[0][row][col].roomSize.x / 2.f;

				// the next 4 points should align with the hallway
				p5.pos.x = p6.pos.x = center.x - hallwayWidth / 2.f;
				p7.pos.x = p8.pos.x = center.x + hallwayWidth / 2.f;

				// Now we calculate the y values
				p1.pos.y = p5.pos.y = p8.pos.y = p4.pos.y = 0.f;
				p2.pos.y = p3.pos.y = hallwayHeight * 1.5f;
				p6.pos.y = p7.pos.y = hallwayHeight;

				p1.pos.z = p2.pos.z = p3.pos.z = p4.pos.z = p5.pos.z = p6.pos.z = p7.pos.z = p8.pos.z = center.z + map[0][row][col].roomSize.y / 2.f;
				// adjusting normals
				p1.nrm = p2.nrm = p3.nrm = p4.nrm = p5.nrm = p6.nrm = p7.nrm = p8.nrm = { 0.f, 0.f, -1.f };
				// TODO CHANGE UV USING FUNCTION WHEN IMPLEMENTED
				p1.uv = { 0.f, 0.f };
				p2.uv = { 0.f, 1.f };
				p3.uv = { 1.f, 1.f };
				p4.uv = { 1.f, 0.f };

				p5.uv = { 0.4f, 0.f };
				p6.uv = { 0.4f, 0.6f };
				p7.uv = { 0.6f, 0.6f };
				p8.uv = { 0.6f, 0.0f };
				currentIndx = roomObj->model.vertices.size();
				if (hasUp) {
					roomObj->model.vertices.push_back(p1);
					roomObj->model.vertices.push_back(p2);
					roomObj->model.vertices.push_back(p3);
					roomObj->model.vertices.push_back(p4);

					roomObj->model.vertices.push_back(p5);
					roomObj->model.vertices.push_back(p6);
					roomObj->model.vertices.push_back(p7);
					roomObj->model.vertices.push_back(p8);

					// triangle 1
					roomObj->model.indices.push_back(currentIndx + 0);
					roomObj->model.indices.push_back(currentIndx + 1);
					roomObj->model.indices.push_back(currentIndx + 4);

					// triangle 2
					roomObj->model.indices.push_back(currentIndx + 4);
					roomObj->model.indices.push_back(currentIndx + 1);
					roomObj->model.indices.push_back(currentIndx + 5);

					// triangle 3
					roomObj->model.indices.push_back(currentIndx + 1);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 5);

					// triangle 4
					roomObj->model.indices.push_back(currentIndx + 5);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 6);

					// triangle 5
					roomObj->model.indices.push_back(currentIndx + 6);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 7);

					// triangle 6
					roomObj->model.indices.push_back(currentIndx + 7);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 3);
				}
				else {
					roomObj->model.vertices.push_back(p1);
					roomObj->model.vertices.push_back(p2);
					roomObj->model.vertices.push_back(p3);
					roomObj->model.vertices.push_back(p4);

					// triangle 1
					roomObj->model.indices.push_back(currentIndx + 0);
					roomObj->model.indices.push_back(currentIndx + 1);
					roomObj->model.indices.push_back(currentIndx + 2);

					// triangle 2
					roomObj->model.indices.push_back(currentIndx + 0);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 3);
				}

				// Down wall
				p1.pos.z = p2.pos.z = p3.pos.z = p4.pos.z = p5.pos.z = p6.pos.z = p7.pos.z = p8.pos.z = center.z - map[0][row][col].roomSize.y / 2.f;
				// adjusting normals
				p1.nrm = p2.nrm = p3.nrm = p4.nrm = p5.nrm = p6.nrm = p7.nrm = p8.nrm = { 0.f, 0.f, 1.f };
				currentIndx = roomObj->model.vertices.size();

				if (hasDown) {
					roomObj->model.vertices.push_back(p1);
					roomObj->model.vertices.push_back(p2);
					roomObj->model.vertices.push_back(p3);
					roomObj->model.vertices.push_back(p4);

					roomObj->model.vertices.push_back(p5);
					roomObj->model.vertices.push_back(p6);
					roomObj->model.vertices.push_back(p7);
					roomObj->model.vertices.push_back(p8);

					// triangle 1
					roomObj->model.indices.push_back(currentIndx + 0);
					roomObj->model.indices.push_back(currentIndx + 1);
					roomObj->model.indices.push_back(currentIndx + 4);

					// triangle 2
					roomObj->model.indices.push_back(currentIndx + 4);
					roomObj->model.indices.push_back(currentIndx + 1);
					roomObj->model.indices.push_back(currentIndx + 5);

					// triangle 3
					roomObj->model.indices.push_back(currentIndx + 1);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 5);

					// triangle 4
					roomObj->model.indices.push_back(currentIndx + 5);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 6);

					// triangle 5
					roomObj->model.indices.push_back(currentIndx + 6);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 7);

					// triangle 6
					roomObj->model.indices.push_back(currentIndx + 7);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 3);
				}
				else {
					roomObj->model.vertices.push_back(p1);
					roomObj->model.vertices.push_back(p2);
					roomObj->model.vertices.push_back(p3);
					roomObj->model.vertices.push_back(p4);

					// triangle 1
					roomObj->model.indices.push_back(currentIndx + 0);
					roomObj->model.indices.push_back(currentIndx + 1);
					roomObj->model.indices.push_back(currentIndx + 2);

					// triangle 2
					roomObj->model.indices.push_back(currentIndx + 0);
					roomObj->model.indices.push_back(currentIndx + 2);
					roomObj->model.indices.push_back(currentIndx + 3);
				}
			}
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
		for (int i = 0; i < 4; ++i) {
			if (neightbors[i] == NEIGHBOR::NONE || neightbors[i] == n) {
				neightbors[i] = n;
				return; 
			} 
		}
	}

	void map_room::activate(glm::vec2 minSize, glm::vec2 maxSize, glm::vec3 position) {
		active = true;
		roomSize = glm::linearRand(minSize, maxSize);

		/*if (roomObjs.empty() || !roomObjs.front()) {
			return;
		}
		auto& ptr = roomObjs.front();
		ptr->model.MakeFloorPlane(roomSize.x, roomSize.y);
		ptr->setPosition(position);
		ptr->resourceId = 7;
		ptr->isActive = true;*/
	}
}