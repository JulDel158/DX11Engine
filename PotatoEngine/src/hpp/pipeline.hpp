#pragma once

// directx11
#include <d3d11_2.h>

// lib
#include <SpriteBatch.h>
#include <SpriteFont.h>

// std
#include <memory>

#include "debug_lines.hpp"
#include "pipeline_properties.hpp"
#include "object_data.hpp"

namespace {
	template<typename T>
	void safe_release(T* t) {
		if (t) { t->Release(); }
	}
}

// TODO: Move perspective matrix to its own buffer
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

		void bindFrameBuffer(const Frame_cb& frameCbuffer,const Scene_cb& sceneCbuffer, const bool invertView = 0);

		void bindWindowBuffer(const Window_cb& windowData);

		void drawDebugLines();

		void drawObject(Objectdata& obj);

		void drawSkybox(const GameObject* skybox);

		void drawGameObjects(const GameObject* gameObjects, uint32_t size);

		void drawText(const Textwrap* set, const uint32_t size);

		void drawParticle();

		void drawCsParticles();

		void setRenderTargetView();

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
		std::vector<ObjVertex>		vBuffer;
		std::vector<ParticleVertex> pvBuffer;

		ID3D11Buffer*				indexBuffer[INDEX_BUFFER::COUNT]{};
		std::vector<uint32_t>		iBuffer;

		ID3D11InputLayout*			inputLayout[INPUT_LAYOUT::COUNT]{};
		 
		ID3D11VertexShader*			vertexShader[VERTEX_SHADER::COUNT]{}; // we will need a couple of buffers of different sizes

		ID3D11PixelShader*			pixelShader[PIXEL_SHADER::COUNT]{};

		ID3D11GeometryShader*		geometryShader[GEOMETRY_SHADER::COUNT]{};

		ID3D11Buffer*				constantBuffer[CONSTANT_BUFFER::COUNT]{};

		D3D11_VIEWPORT				viewPort[VIEWPORT::COUNT]{};

		ID3D11ShaderResourceView*	sResourceView[SUBRESOURCE_VIEW::COUNT]{};  // may be moved out of here for textures

		ID3D11SamplerState*			samplerState[SAMPLER_STATE::COUNT]{};

		ID3D11BlendState*			blendState[BLEND_STATE::COUNT]{};

		ID3D11Buffer*				resourceBuffer[RESOURCE_BUFFER::COUNT]{};

		ID3D11UnorderedAccessView*  uAccessView[UACCESS_VIEW::COUNT]{};

		ID3D11ComputeShader*		computeShader[COMPUTE_SHADER::COUNT]{};

		// This classes are used to render fonts
		std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
		std::unique_ptr<DirectX::SpriteFont> spriteFont;


		void createDeviceAndSwapChain();

		void createMainRenderTarget();

		void setUpDepthStencil();

		void setUpRasterizer();

		void createSamplerState();

		void createShaderAndInputLayout();

		void createVertexBuffers();

		void createConstantBuffers();

		// remember to use the format L"filename" when calling this function
		void createSResourceView(const wchar_t* filename, const uint32_t index);

		void createDebugTexture();

		void createBlendStates();

		void createParticleBuffers();

		void createComputeShaders();

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