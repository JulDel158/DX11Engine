#pragma once

#include <array>
#include <DirectXMath.h>

#include "object_data.hpp"

namespace dxe {

	struct debug_lines {
		debug_lines() = delete;
		~debug_lines() = delete;
		debug_lines(const debug_lines&) = delete;
		debug_lines& operator=(const debug_lines&) = delete;

		static void clearLines();

		static void addLine(DirectX::XMFLOAT3 apos, DirectX::XMFLOAT3 bpos, DirectX::XMFLOAT4 acolor, DirectX::XMFLOAT4 bcolor);

		inline static void addLine(DirectX::XMFLOAT3 apos, DirectX::XMFLOAT3 bpos, DirectX::XMFLOAT4 color) { addLine(apos, bpos, color, color); }

		static const ColoredVertex* getLineVerts();

		static size_t getLineVertCount();

		static size_t getLineVertCapacity();

		static void addGrid();
	};

} // namespace dxe