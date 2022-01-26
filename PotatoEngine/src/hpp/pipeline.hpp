#pragma once

// directx11
#include <d3d11_2.h>

#include "pipeline_properties.hpp"

namespace {
	template<typename T>
	void safe_release(T* t) {
		if (t) { t->Release(); }
	}
}

namespace dxe {

	class pipeline {
	public:
		pipeline(HWND windowHandle);
		~pipeline();

		pipeline(const pipeline&) = delete;
		pipeline &operator=(const pipeline&) = delete;
		pipeline(const pipeline&&) = delete;
		pipeline& operator=(const pipeline&&) = delete;

		void present(unsigned int vsync);

		void drawDebugLines();

	private:
		HWND hwnd;

		ID3D11Device*				device = nullptr;
		ID3D11DeviceContext*		context = nullptr;
		IDXGISwapChain*				swapChain = nullptr;

		ID3D11RenderTargetView*		renderTarget[RENDER_TARGET_VIEW::COUNT]{};
		
		ID3D11DepthStencilView*		depthStencilView[DEPTH_STENCIL_VIEW::COUNT]{};

		ID3D11DepthStencilState*	depthStencilState[DEPTH_STENCIL_STATE::COUNT]{};

		ID3D11RasterizerState*		rasterState[RASTER_STATE::COUNT]{};

		ID3D11Buffer*				vertexBuffer[VERTEX_BUFFER::COUNT]{};

		ID3D11Buffer*				indexBuffer[INDEX_BUFFER::COUNT]{};

		ID3D11InputLayout*			inputLayout[INPUT_LAYOUT::COUNT]{};
		 
		ID3D11VertexShader*			vertexShader[VERTEX_SHADER::COUNT]{}; // we will need a couple of buffers of different sizes

		ID3D11PixelShader*			pixelShader[PIXEL_SHADER::COUNT]{};

		ID3D11Buffer*				constantBuffer[CONSTANT_BUFFER::COUNT]{};

		D3D11_VIEWPORT				view_port[VIEWPORT::COUNT]{};

		ID3D11ShaderResourceView*	sResourceView[SUBRESOURCE_VIEW::COUNT]{};  // may be moved out of here for textures

		ID3D11SamplerState*			samplerState[SAMPLER_STATE::COUNT]{};


		void createDeviceAndSwapChain();

		void createMainRenderTarget();

		void setUpDepthStencil();

		void setUpRasterizer();

		void createSamplerState();

		void createShaderAndInputLayout();

		void createVertexBuffers();

		void createConstantBuffers();

	};

	/* IMPORTANT: 
	*  Creation T/B and destruction B/T order:
	*  Create device
	*  Create swapchain
	*  Create device context
	*  Create render target view
	*  Create depth stencil
	*  Create raster state
	*  Create sampler state
	*  Create vertex shader
	*  Create pixel shader
	*  Create input layout
	*  Create vertex buffers
	*  Create index buffers
	*  Create Constant buffers
	*  Create textures/subresource view
	*/

} // namespace dxe