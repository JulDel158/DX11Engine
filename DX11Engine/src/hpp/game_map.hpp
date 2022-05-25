#pragma once

#include <glm/glm.hpp>

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
		glm::mat4 pos{ 1.f };
		// TODO:: add list of game objects to render

		inline void clear() { active = false; type = ROOM_TYPE::DEBUG; };
		inline void addNeighbor(NEIGHBOR n) { for (int i = 0; i < MAX_NEIGHBOR_COUNT; ++i) { if (neightbors[i] == NEIGHBOR::NONE || neightbors[i] == n) { neightbors[i] = n; return; } } }
	};

	class game_map
	{
	public:
		game_map(int width, int height, int maxFloorCount, int startingMinimunRoomCount, glm::vec2 cellDimension);
		~game_map();

		void clearFloor(int floor);
		void clearSmallestFloor();
		void generateNextFloor(); // clears the oldest floor and generates a new one within the same block of memory
		void printMapData();

	private:
		void generateDungeon();
		void randomWalkGeneration(map_room**& floor);

		int width{ 0 };
		int height{ 0 };
		int maxFloorCount{ 0 };
		int minRoomCount{ 0 };
		glm::vec2 cellDimension{ 0.f };
		int* floorIds{ nullptr };
		map_room*** map{ nullptr };
	};

} // namespace dxe