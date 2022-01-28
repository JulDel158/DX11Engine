#include "../hpp/pipeline.hpp"

// lib
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <cassert>
#include <stdexcept>
#include <vector>

// dep
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
// #pragma comment(lib, "DXGI.lib")

#include "../hpp/file_reader.hpp"

namespace dxe {

	pipeline::pipeline(HWND windowHandle) : hwnd{ windowHandle } {
		createDeviceAndSwapChain();

		createMainRenderTarget();

		setUpDepthStencil();

		setUpRasterizer();

		createSamplerState();

		createShaderAndInputLayout();

		createVertexBuffers();

		createConstantBuffers();

		float aspect = viewPort[VIEWPORT::DEFAULT].Width / viewPort[VIEWPORT::DEFAULT].Height;
		
		//tempView.FPSViewRH({ 0.f, 3.f, -15.f }, glm::radians(-30.f), glm::radians(0.f));
		//// tempView.view = glm::inverse(tempView.view);
		//// tempView.dPrintViewMat();
		//tempView.view = glm::mat4{1.f};
		//tempView.LookAtTarget({ 0.f, 5.f, -15.f }, { 0.f, 5.f, 0.f }, {0.f, 1.f, 0.f});
		//tempView.view = glm::inverse(tempView.view);
		//tempView.dPrintViewMat();

		//tempView.setPerspectiveProjection(glm::pi<float>() / 4.f, aspect, 0.01f, 100.f);
	}

	pipeline::~pipeline() {

		for (auto& ptr : sResourceView) { // later on textures will be created and released somewhere else
			safe_release(ptr);
		}

		for (auto& ptr : constantBuffer) {
			safe_release(ptr);
		}

		for (auto& ptr : indexBuffer) {
			safe_release(ptr);
		}

		for (auto& ptr : vertexBuffer) {
			safe_release(ptr);
		}

		for (auto& ptr : inputLayout) {
			safe_release(ptr);
		}

		for (auto& ptr : pixelShader) {
			safe_release(ptr);
		}

		for (auto& ptr : vertexShader) {
			safe_release(ptr);
		}

		for (auto& ptr : samplerState) {
			safe_release(ptr);
		}

		for (auto& ptr : rasterState) {
			safe_release(ptr);
		}

		for (auto& ptr : depthStencilState) {
			safe_release(ptr);
		}

		for (auto& ptr : depthStencilView) {
			safe_release(ptr);
		}

		for (auto& ptr : renderTarget) {
			safe_release(ptr);
		}

		safe_release(context);

		safe_release(swapChain);

		safe_release(device);
	}

	void pipeline::present(unsigned int vsync) {
		swapChain->Present(vsync, 0);
	}

	void pipeline::bindFrameBuffer(const View_t& viewProj) {
		Frame_cb cb{ viewProj.view, viewProj.projection };
		context->UpdateSubresource(constantBuffer[CONSTANT_BUFFER::FRAME_CB], 0, NULL, &cb, 0, 0);
	}

	void pipeline::drawDebugLines() {
		UINT stride = sizeof(ColoredVertex);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, &vertexBuffer[VERTEX_BUFFER::COLORED_LINES], &stride, &offset);

		context->UpdateSubresource(vertexBuffer[VERTEX_BUFFER::COLORED_LINES], 0, nullptr, debug_lines::getLineVerts(), 0, 0);

		context->IASetInputLayout(inputLayout[INPUT_LAYOUT::DEFAULT]);

		context->VSSetShader(vertexShader[VERTEX_SHADER::DEFAULT], nullptr, 0);

		context->VSSetConstantBuffers(0, 1, &constantBuffer[CONSTANT_BUFFER::OBJECT_CB]);

		context->VSSetConstantBuffers(1, 1, &constantBuffer[CONSTANT_BUFFER::FRAME_CB]);

		context->PSSetShader(pixelShader[PIXEL_SHADER::DEFAULT], nullptr, 0);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		Object_cb cb1;
		cb1.modeling = glm::mat4{ 1.f };

		context->UpdateSubresource(constantBuffer[CONSTANT_BUFFER::OBJECT_CB], 0, NULL, &cb1, 0, 0);

		context->Draw(debug_lines::getLineVertCount(), 0);
		debug_lines::clearLines();
	}

	void pipeline::createDeviceAndSwapChain() {
		RECT crect;
		GetClientRect(hwnd, &crect); // Retrieves the coordinates of a window's client area. The client coordinates specify the upper-left and lower-right corners

		// Set up viewport
		D3D11_VIEWPORT& vp = viewPort[VIEWPORT::DEFAULT];
		vp.Width = static_cast<float>(crect.right);
		vp.Height = static_cast<float>(crect.bottom);
		vp.MinDepth = 0.f;
		vp.MaxDepth = 1.f;
		vp.TopLeftX = 0.f;
		vp.TopLeftY = 0.f;

		// Setup Swap Chain
		DXGI_SWAP_CHAIN_DESC sd{ 0 };
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = crect.right;
		sd.BufferDesc.Height = crect.bottom;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hwnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		
		D3D_FEATURE_LEVEL featureLevelsSupported;

		const D3D_FEATURE_LEVEL lvl[] = {
			D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_9_3, D3D_FEATURE_LEVEL_9_1
		};

		UINT createDeviceFlags = 0;

#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

		// creating device, swapchain and context
		HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
			createDeviceFlags, lvl, _countof(lvl), D3D11_SDK_VERSION, &sd, &swapChain, &device, &featureLevelsSupported, &context);

		if (hr == E_INVALIDARG) {
			hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
				createDeviceFlags, &lvl[1], _countof(lvl) - 1, D3D11_SDK_VERSION, &sd, &swapChain, &device, &featureLevelsSupported, &context);
		}

		if (FAILED(hr)) {
			throw std::runtime_error("Device, Swap Chain, and Device Context creation failed!");
		}
	}

	void pipeline::createMainRenderTarget() {
		ID3D11Texture2D* pBackBuffer;

		// get a pointer to the back buffer
		HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer); // could we use a different type of cast?

		if (FAILED(hr)) {
			throw std::runtime_error("Call to swapChain->GetBuffer failed!");
		}

		// create render-target view
		hr = device->CreateRenderTargetView(pBackBuffer, NULL, &renderTarget[RENDER_TARGET_VIEW::DEFAULT]);

		if (FAILED(hr)) {
			throw std::runtime_error("Create Render Target view failed!");
		}

		safe_release(pBackBuffer);
	}

	void pipeline::setUpDepthStencil() {
		// DEPTH BUFFER
		D3D11_TEXTURE2D_DESC depthBufferDesc{ 0 };
		ID3D11Texture2D* depthStencilBuffer;

		depthBufferDesc.Width = static_cast<UINT>(viewPort[VIEWPORT::DEFAULT].Width);
		depthBufferDesc.Height = static_cast<UINT>(viewPort[VIEWPORT::DEFAULT].Height);
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		HRESULT hr = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);

		if (FAILED(hr)) {
			throw std::runtime_error("Create Depth Stencil buffer failed!");
		}

		/* DEPTH_STENCIL */
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		hr = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView[DEPTH_STENCIL_VIEW::DEFAULT]);

		if (FAILED(hr)) {
			throw std::runtime_error("Create Depth Stencil View failed!");
		}

		safe_release(depthStencilBuffer);

		/* DEPTH_STENCIL_DESC */
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		hr = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState[DEPTH_STENCIL_STATE::DEFAULT]);

		if (FAILED(hr)) {
			throw std::runtime_error("Create Depth Stencil State failed!");
		}
	}

	void pipeline::setUpRasterizer() {
		D3D11_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(rasterDesc));

		rasterDesc.AntialiasedLineEnable = true;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = false;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = false;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		HRESULT hr = device->CreateRasterizerState(&rasterDesc, &rasterState[RASTER_STATE::DEFAULT]);

		if (FAILED(hr)) {
			throw std::runtime_error("Create Rasterizer State failed!");
		}
	}

	void pipeline::createSamplerState() {
		D3D11_SAMPLER_DESC sd = {};
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sd.MinLOD = 0;
		sd.MaxLOD = D3D11_FLOAT32_MAX;
		HRESULT hr = device->CreateSamplerState(&sd, &samplerState[SAMPLER_STATE::DEFAULT]);

		if (FAILED(hr)) {
			throw std::runtime_error("Create Sampler State failed!");
		}
	}

	void pipeline::createShaderAndInputLayout() {
		std::vector<uint8_t> vs_blob = tools::file_reader::load_binary_blob("shaders/simple_vertex_shader.cso");
		std::vector<uint8_t> ps_blob = tools::file_reader::load_binary_blob("shaders/simple_pixel_shader.cso");

		HRESULT hr = device->CreateVertexShader(vs_blob.data(), vs_blob.size(), NULL, &vertexShader[VERTEX_SHADER::DEFAULT]);
		assert(!FAILED(hr) && "failed to create vertex shader");

		hr = device->CreatePixelShader(ps_blob.data(), ps_blob.size(), NULL, &pixelShader[PIXEL_SHADER::DEFAULT]);
		assert(!FAILED(hr) && "failed to create pixel shader");

		D3D11_INPUT_ELEMENT_DESC idesc[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		hr = device->CreateInputLayout(idesc, ARRAYSIZE(idesc), vs_blob.data(), vs_blob.size(), &inputLayout[INPUT_LAYOUT::DEFAULT]);
		assert(!FAILED(hr) && "failed to create input layout");
	}

	void pipeline::createVertexBuffers() {
		HRESULT hr;
		
		CD3D11_BUFFER_DESC desc = CD3D11_BUFFER_DESC(
			sizeof(ColoredVertex) * debug_lines::getLineVertCapacity(),
			D3D11_BIND_VERTEX_BUFFER);

		D3D11_SUBRESOURCE_DATA srd = { 0 };
		srd.pSysMem = debug_lines::getLineVerts();

		hr = device->CreateBuffer(&desc, &srd, &vertexBuffer[VERTEX_BUFFER::COLORED_LINES]);
		assert(!FAILED(hr) && "failed to create input layout");
	}

	void pipeline::createConstantBuffers() {
		D3D11_BUFFER_DESC obj_cb;
		ZeroMemory(&obj_cb, sizeof(obj_cb));

		obj_cb.Usage = D3D11_USAGE_DEFAULT;
		obj_cb.ByteWidth = sizeof(Object_cb);
		obj_cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		obj_cb.CPUAccessFlags = 0;

		HRESULT hr = device->CreateBuffer(&obj_cb, NULL, &constantBuffer[CONSTANT_BUFFER::OBJECT_CB]);
		assert(!FAILED(hr) && "failed to create constant buffer: Object_cb");

		D3D11_BUFFER_DESC frm_cb;
		ZeroMemory(&frm_cb, sizeof(frm_cb));

		frm_cb.Usage = D3D11_USAGE_DEFAULT;
		frm_cb.ByteWidth = sizeof(Frame_cb);
		frm_cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		frm_cb.CPUAccessFlags = 0;

		hr = device->CreateBuffer(&frm_cb, NULL, &constantBuffer[CONSTANT_BUFFER::FRAME_CB]);
		assert(!FAILED(hr) && "failed to create constant buffer: Frame_cb");
	}

	void pipeline::setRenderTargetView() {
		const glm::vec4 black{ 0.f, 0.f, 0.f, 1.f };

		context->OMSetDepthStencilState(depthStencilState[DEPTH_STENCIL_STATE::DEFAULT], 1);
		context->OMSetRenderTargets(1, &renderTarget[RENDER_TARGET_VIEW::DEFAULT], depthStencilView[DEPTH_STENCIL_VIEW::DEFAULT]);

		context->ClearRenderTargetView(renderTarget[RENDER_TARGET_VIEW::DEFAULT], &black.x);
		context->ClearDepthStencilView(depthStencilView[DEPTH_STENCIL_VIEW::DEFAULT], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		context->RSSetState(rasterState[RASTER_STATE::DEFAULT]);
		context->RSSetViewports(1, &viewPort[VIEWPORT::DEFAULT]);
	}

} // namespace dxe