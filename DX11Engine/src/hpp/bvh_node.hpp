#pragma once

#include <stdint.h>
#include "collision.hpp"

namespace dxe {


	class bvh_node {
	public:
		bvh_node(uint32_t left_index, uint32_t right_index) : _left{ left_index }, _right{ right_index } {}
		bvh_node(const aabb_t& bounds, uint32_t id) : _aabb{ bounds }, _id{ id }, _left{ 0 } {}

		bvh_node() : _left{ 0 }, _right{ 0 } {}
		~bvh_node() = default;
		bvh_node(const bvh_node&) = default;
		bvh_node& operator=(const bvh_node&) = default;

		// root must be at [0], so parent index must be invalid
		bool isRoot()const { return _parent == UINT32_MAX; }

		bool isBranch()const { return is_branch; }

		bool isLeaf()const { return !isBranch(); }

		uint32_t elementId()const {
			//assert(isLeaf());
			return _id;
		}

		uint32_t& left() {
			//assert(isBranch());
			return _left;
		}

		uint32_t left()const {
			assert(isBranch());
			return _left;
		}

		uint32_t& right() {
			//assert(isBranch());
			return _right;
		}

		uint32_t right() const {
			//assert(isBranch());
			return _right;
		}

		uint32_t parent()const {
			//assert(!isRoot());
			return _parent;
		}
		
		uint32_t& parent() {
			assert(!isRoot());
			return _parent;
		}

		aabb_t& aabb() {
			return _aabb;
		}

		const aabb_t& aabb()const {
			return _aabb;
		}


	private:
		uint32_t _parent{ UINT32_MAX };

		union {
			uint32_t _left;
			uint32_t is_branch;
		};

		union {
			uint32_t _right;
			uint32_t _id;
		};

		aabb_t _aabb;
			
	};

} // namespace dxe