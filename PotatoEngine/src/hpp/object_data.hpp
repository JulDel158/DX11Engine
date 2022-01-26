#pragma once
#include <DirectXMath.h>

namespace dxe {

	struct Object_cb {
		DirectX::XMMATRIX modeling;
	};

	struct Frame_cb {
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct ColoredVertex {
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT4 color;
	};

} // namespace dxe