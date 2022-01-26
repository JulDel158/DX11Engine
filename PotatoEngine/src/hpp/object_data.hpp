#pragma once
#include <DirectXMath.h>

namespace dxe {

	struct alignas(16)ColoredVertex {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
	};

	struct View_t {
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

	struct Object_cb {
		DirectX::XMFLOAT4X4 modeling;
	};

	struct Frame_cb {
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

} // namespace dxe