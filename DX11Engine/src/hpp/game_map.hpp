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
		static constexpr int MAX_NEIGHBOR_COUNT = 4;
		NEIGHBOR neightbors[MAX_NEIGHBOR_COUNT] = { NEIGHBOR::NONE, NEIGHBOR::NONE, NEIGHBOR::NONE, NEIGHBOR::NONE };
		//glm::mat4 pos{ 1.f };
		// TODO:: add list of game objects to render each room will have a pointer to the meshes that represent their floor, walls, and celing 
		
		std::vector<GameObject*> roomObjs;

		inline void clear() { active = false; type = ROOM_TYPE::DEBUG; };
		inline void addNeighbor(NEIGHBOR n) { for (int i = 0; i < MAX_NEIGHBOR_COUNT; ++i) { if (neightbors[i] == NEIGHBOR::NONE || neightbors[i] == n) { neightbors[i] = n; return; } } }
		inline void activate() { active = true; }
	};

	class game_map
	{
	public:
		game_map(uint64_t width, uint64_t height, uint64_t maxFloorCount, uint64_t startingMinimunRoomCount, glm::vec2 cellDimension);
		~game_map();

		void clearFloor(uint64_t floor);
		void clearSmallestFloor();
		void generateNextFloor(); // clears the oldest floor and generates a new one within the same block of memory
		void printMapData();
		void generateRoomMeshes(GameObject*& buffer, uint64_t startPos, uint64_t size);
		uint64_t getRequiredMeshCount(); // returns the number of meshes required to form each potential room
		

	private:
		void generateDungeon();
		void randomWalkGeneration(map_room**& floor);
		void initializeMidPoints();

		static constexpr uint64_t MESH_PER_ROOM_COUNT = 1;
		uint64_t width{ 0 };
		uint64_t height{ 0 };
		uint64_t maxFloorCount{ 0 };
		uint64_t minRoomCount{ 0 };
		glm::vec2 cellDimension{ 0.f };
		int* floorIds{ nullptr };
		map_room*** map{ nullptr };
		glm::mat4** midpoints{ nullptr };
	};

} // namespace dxe