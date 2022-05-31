#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "object_data.hpp"

namespace dxe {

	// the map will consist of a grid of rooms, to avoid allocations and deallocations at run time, the dungeon will consist of a n number floors at any given time, which means every time we go to a new floor
	// we clear the olders floor and replace generate a new layout with the already allocated memory for the dungeon

	enum class ROOM_TYPE {
		DEBUG = 0, START = 1, END, RANDOM, KEY, LOCK, TREASSURE, SECRET, BOSS
	};

	enum class NEIGHBOR { // used to identify which rooms a given room is connected to 
		NONE = 0, UP, DOWN, LEFT, RIGHT
	};

	struct map_room {
		bool active{ false };
		ROOM_TYPE type{ ROOM_TYPE::DEBUG };
		glm::vec2 roomSize{ 0.f }; // may be changed to a vec 3 later to handle the celing 
		static constexpr int MAX_NEIGHBOR_COUNT = 4;
		NEIGHBOR neightbors[MAX_NEIGHBOR_COUNT] = { NEIGHBOR::NONE, NEIGHBOR::NONE, NEIGHBOR::NONE, NEIGHBOR::NONE }; 
		std::vector<GameObject*> roomObjs;

		void clear();
		void addNeighbor(NEIGHBOR n);
		void activate(glm::vec2 minSize, glm::vec2 maxSize, glm::vec3 position);
	};

	class game_map
	{
	public:
		game_map(uint64_t width, uint64_t height, uint64_t maxFloorCount, uint64_t startingMinimunRoomCount, glm::vec2 maxCellDimension, glm::vec2 minCellDimension, float offset);
		~game_map();

		void clearFloor(uint64_t floor);
		void clearSmallestFloor();
		void generateNextFloor(); // clears the oldest floor and generates a new one within the same block of memory
		void printMapData();
		void generateRoomMeshes(GameObject*& buffer, uint64_t startPos, uint64_t size);
		uint64_t getRequiredMeshCount(); // returns the number of meshes required to form each potential room
	
		void generateDungeon();

	private:
		void randomWalkGeneration(map_room**& floor);
		void initializeMidPoints();
		void generateHallwayMeshes();

		static constexpr uint64_t MESH_PER_ROOM_COUNT = 1;
		uint64_t gridWidth{ 0 };
		uint64_t gridHeight{ 0 };
		uint64_t maxFloorCount{ 0 };
		uint64_t minRoomCount{ 0 };
		glm::vec2 maxCellDimension{ 0.f };
		glm::vec2 minCellDimension{ 0.f };
		float roomOffset{ 0.f };
		int* floorIds{ nullptr };
		map_room*** map{ nullptr };
		GameObject** hallways{ nullptr };
		glm::mat4** midpoints{ nullptr };
	};

} // namespace dxe

// map must have an offset distance which is the minimun space between 2 rooms
// midpoints must be calculated based on offset
// every room now stores its own size (this will be used to determine the cull planes used to render the hallways)
// a set of hallways is generated based on the dimensions of the grid and max space
// a minimun room dimmension size is used as the width of all the hallways