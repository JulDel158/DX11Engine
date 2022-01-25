#include "../hpp/pipeline.hpp"

#include <cassert>
#include <stdexcept>

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

	void pipeline::createDeviceAndSwapChain() {
		RECT crect;
		GetClientRect(hwnd, &crect); // Retrieves the coordinates of a window's client area. The client coordinates specify the upper-left and lower-right corners

		// Set up viewport
		D3D11_VIEWPORT& vp = view_port[VIEWPORT::DEFAULT];
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

		depthBufferDesc.Width = static_cast<UINT>(view_port[VIEWPORT::DEFAULT].Width);
		depthBufferDesc.Height = static_cast<UINT>(view_port[VIEWPORT::DEFAULT].Height);
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

	}

	void pipeline::createSamplerState() {

	}

	void pipeline::createShaderAndInputLayout() {

	}

	void pipeline::createVertexBuffers() {

	}

	void pipeline::createConstantBuffers() {

	}

} // namespace dxe