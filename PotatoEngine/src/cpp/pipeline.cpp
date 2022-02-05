#include "../hpp/pipeline.hpp"

// lib
#include <DDSTextureLoader.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
//#include <SpriteBatch.h>
//#include <SpriteFont.h>


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

		// temporary
		createSResourceView(L"assets/images/obamium.dds", static_cast<uint32_t>(SUBRESOURCE_VIEW::DEFAULT));

		createSResourceView(L"assets/images/SkyboxOcean.dds", static_cast<uint32_t>(SUBRESOURCE_VIEW::SKYBOX));

		createDebugTexture();
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

	void pipeline::bindFrameBuffer(const Frame_cb& frameCbuffer, const Scene_cb& sceneCbuffer, const bool invertView) {
		Frame_cb cb{ frameCbuffer };
		if (invertView) {
			cb.view = glm::inverse(cb.view);
		}

		context->VSSetConstantBuffers(1, 1, &constantBuffer[CONSTANT_BUFFER::FRAME_CB]);

		context->UpdateSubresource(constantBuffer[CONSTANT_BUFFER::FRAME_CB], 0, NULL, &cb, 0, 0);

		// BINDING TEMPORARY SCENE BUFFER HERE, TO BE REMOVED

		context->PSSetConstantBuffers(3, 1, &constantBuffer[CONSTANT_BUFFER::SCENE_CB]);

		context->UpdateSubresource(constantBuffer[CONSTANT_BUFFER::SCENE_CB], 0, NULL, &sceneCbuffer, 0, 0);
	}

	void pipeline::bindWindowBuffer(const Window_cb& wcb) {
		context->VSSetConstantBuffers(2, 1, &constantBuffer[CONSTANT_BUFFER::WINDOW_CB]);

		context->UpdateSubresource(constantBuffer[CONSTANT_BUFFER::WINDOW_CB], 0, NULL, &wcb, 0, 0);
	}

	void pipeline::drawDebugLines() {
		UINT stride = sizeof(ColoredVertex);
		UINT offset = 0;

		context->IASetInputLayout(inputLayout[INPUT_LAYOUT::DEFAULT]);

		context->IASetVertexBuffers(0, 1, &vertexBuffer[VERTEX_BUFFER::COLORED_LINES], &stride, &offset);

		context->UpdateSubresource(vertexBuffer[VERTEX_BUFFER::COLORED_LINES], 0, nullptr, debug_lines::getLineVerts(), 0, 0);

		context->VSSetShader(vertexShader[VERTEX_SHADER::DEFAULT], nullptr, 0);

		context->PSSetShader(pixelShader[PIXEL_SHADER::DEFAULT], nullptr, 0);

		Object_cb cb1;
		cb1.modeling = glm::mat4{ 1.f };

		context->VSSetConstantBuffers(0, 1, &constantBuffer[CONSTANT_BUFFER::OBJECT_CB]);

		context->UpdateSubresource(constantBuffer[CONSTANT_BUFFER::OBJECT_CB], 0, NULL, &cb1, 0, 0);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		context->Draw(static_cast<UINT>(debug_lines::getLineVertCount()), 0);

		debug_lines::clearLines();
	}

	void pipeline::drawObject(Objectdata& obj) {
		UINT stride = sizeof(ObjVertex);
		UINT offset = 0;

		context->IASetInputLayout(inputLayout[INPUT_LAYOUT::OBJECT]);
		
		// SETTING BUFFERS
		context->IASetVertexBuffers(0, 1, &vertexBuffer[VERTEX_BUFFER::OBJ_40000], &stride, &offset);

		vBuffer = obj.vertices; // we must first copy the data into the buffer
		context->UpdateSubresource(vertexBuffer[VERTEX_BUFFER::OBJ_40000], 0, NULL, vBuffer.data(), 0, 0); /*TODO PUT OBJECT VERTEX BUFFER DATA HERE*/

		context->IASetIndexBuffer(indexBuffer[INDEX_BUFFER::OBJ_40000], DXGI_FORMAT_R32_UINT, 0);

		iBuffer = obj.indices;
		context->UpdateSubresource(indexBuffer[INDEX_BUFFER::OBJ_40000], 0, NULL, iBuffer.data(), 0, 0); /*TODO PUT OBJECT INDEX BUFFER DATA HERE*/

		Object_cb cb;
		cb.modeling = glm::mat4{ 1.f };

		context->VSSetConstantBuffers(0, 1, &constantBuffer[CONSTANT_BUFFER::OBJECT_CB]);

		context->UpdateSubresource(constantBuffer[CONSTANT_BUFFER::OBJECT_CB], 0, NULL, &cb, 0, 0);

		// SETTING SHADERS 
		context->VSSetShader(vertexShader[VERTEX_SHADER::OBJECT], NULL, 0);

		context->PSSetShader(pixelShader[PIXEL_SHADER::OBJ_TEXTURE], NULL, 0);

		// SETTING RESOURCES
		context->PSSetShaderResources(0, 1, &sResourceView[SUBRESOURCE_VIEW::DEBUG]); // TODO: THIS FUNCTION CALL IS TO BIND TEXTURES TO THE PIXEL SHADER, TO BE USED LATER

		context->PSSetSamplers(0, 1, &samplerState[SAMPLER_STATE::DEFAULT]);

		// TIME TO DRAW
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->DrawIndexed(static_cast<UINT>(obj.indices.size()), 0, 0); /*TODO: ADD INDEX COUNT (SIZEOF INDICES)*/
	}

	void pipeline::drawSkybox(const GameObject* skybox) {
		UINT stride = sizeof(ObjVertex);
		UINT offset = 0;

		context->IASetInputLayout(inputLayout[INPUT_LAYOUT::OBJECT]);

		context->IASetVertexBuffers(0, 1, &vertexBuffer[VERTEX_BUFFER::OBJ_40000], &stride, &offset);

		context->IASetIndexBuffer(indexBuffer[INDEX_BUFFER::OBJ_40000], DXGI_FORMAT_R32_UINT, 0);

		context->VSSetConstantBuffers(0, 1, &constantBuffer[CONSTANT_BUFFER::OBJECT_CB]);

		// SETTING SHADERS 
		context->VSSetShader(vertexShader[VERTEX_SHADER::SKYBOX], NULL, 0);

		context->PSSetShader(pixelShader[PIXEL_SHADER::SKYBOX], NULL, 0);

		context->PSSetSamplers(0, 1, &samplerState[SAMPLER_STATE::DEFAULT]);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Object_cb scb;
		scb.modeling = glm::mat4{ 1.f };

		vBuffer = skybox->model.vertices;
		context->UpdateSubresource(vertexBuffer[VERTEX_BUFFER::OBJ_40000], 0, NULL, vBuffer.data(), 0, 0);

		iBuffer = skybox->model.indices;
		context->UpdateSubresource(indexBuffer[INDEX_BUFFER::OBJ_40000], 0, NULL, iBuffer.data(), 0, 0);

		scb.modeling = skybox->transform;
		context->UpdateSubresource(constantBuffer[CONSTANT_BUFFER::OBJECT_CB], 0, NULL, &scb, 0, 0);

		const uint32_t inx = (skybox->resourceId < SUBRESOURCE_VIEW::COUNT) ? skybox->resourceId : SUBRESOURCE_VIEW::DEBUG;
		context->PSSetShaderResources(0, 1, &sResourceView[inx]);

		context->DrawIndexed(static_cast<UINT>(skybox->model.indices.size()), 0, 0); 

		// must clear the depth stencil so we can draw objects "inside" the skybox BEWARE of when this function is call
		context->ClearDepthStencilView(depthStencilView[DEPTH_STENCIL_VIEW::DEFAULT], D3D11_CLEAR_DEPTH, 1.f, 0);
	}

	void pipeline::drawGameObjects(const GameObject* gameObjects, uint32_t size) {
		UINT stride = sizeof(ObjVertex);
		UINT offset = 0;

		context->IASetInputLayout(inputLayout[INPUT_LAYOUT::OBJECT]);

		context->IASetVertexBuffers(0, 1, &vertexBuffer[VERTEX_BUFFER::OBJ_40000], &stride, &offset);

		context->IASetIndexBuffer(indexBuffer[INDEX_BUFFER::OBJ_40000], DXGI_FORMAT_R32_UINT, 0);

		context->VSSetConstantBuffers(0, 1, &constantBuffer[CONSTANT_BUFFER::OBJECT_CB]);

		// SETTING SHADERS 
		context->VSSetShader(vertexShader[VERTEX_SHADER::OBJECT], NULL, 0);

		context->PSSetShader(pixelShader[PIXEL_SHADER::OBJ_TEXTURE], NULL, 0);

		context->PSSetSamplers(0, 1, &samplerState[SAMPLER_STATE::DEFAULT]);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		static Object_cb ocb;
		ocb.modeling = glm::mat4{ 1.f };

		for (size_t i = 0; i < size; ++i) {
			if (!gameObjects[i].isActive) { continue; }

			vBuffer = gameObjects[i].model.vertices;
			context->UpdateSubresource(vertexBuffer[VERTEX_BUFFER::OBJ_40000], 0, NULL, vBuffer.data(), 0, 0);

			iBuffer = gameObjects[i].model.indices;
			context->UpdateSubresource(indexBuffer[INDEX_BUFFER::OBJ_40000], 0, NULL, iBuffer.data(), 0, 0);

			ocb.modeling = gameObjects[i].transform;
			context->UpdateSubresource(constantBuffer[CONSTANT_BUFFER::OBJECT_CB], 0, NULL, &ocb, 0, 0);

			const uint32_t inx = (gameObjects[i].resourceId < SUBRESOURCE_VIEW::COUNT) ? gameObjects[i].resourceId : SUBRESOURCE_VIEW::DEBUG;
			context->PSSetShaderResources(0, 1, &sResourceView[inx]);

			context->DrawIndexed(static_cast<UINT>(gameObjects[i].model.indices.size()), 0, 0);
		}
	}

	void pipeline::createDeviceAndSwapChain() {
		RECT crect;
		GetClientRect(hwnd, &crect); // Retrieves the coordinates of a window's client area. The client coordinates specify the upper-left and lower-right corners

		// Set up viewport
		D3D11_VIEWPORT& vp = viewPort[VIEWPORT::DEFAULT];
		vp.Width = static_cast<float>(crect.right);
		vp.Height = static_cast<float>(crect.bottom);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;

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
		//sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		
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
		D3D11_TEXTURE2D_DESC depthBufferDesc = { 0 };
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
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;;
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		

		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		hr = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView[DEPTH_STENCIL_VIEW::DEFAULT]);

		if (FAILED(hr)) {
			throw std::runtime_error("Create Depth Stencil View failed!");
		}

		safe_release(depthStencilBuffer);

		/* DEPTH_STENCIL_DESC */
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc{ 0 };

		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		

		hr = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState[DEPTH_STENCIL_STATE::DEFAULT]);

		if (FAILED(hr)) {
			throw std::runtime_error("Create Depth Stencil State failed!");
		}
	}

	void pipeline::setUpRasterizer() {
		D3D11_RASTERIZER_DESC rasterDesc;
		ZeroMemory(&rasterDesc, sizeof(rasterDesc));

		rasterDesc.AntialiasedLineEnable = true;
		rasterDesc.CullMode = D3D11_CULL_NONE;
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
		// vertex shaders loading
		std::vector<uint8_t> svs_blob = tools::file_reader::load_binary_blob("shaders/simple_vertex_shader.cso");
		std::vector<uint8_t> ovs_blob = tools::file_reader::load_binary_blob("shaders/obj_vertex_shader.cso");
		std::vector<uint8_t> vss_blob = tools::file_reader::load_binary_blob("shaders/vs_skybox.cso");

		// pixel shaders loading
		std::vector<uint8_t> sps_blob = tools::file_reader::load_binary_blob("shaders/simple_pixel_shader.cso");
		std::vector<uint8_t> onps_blob = tools::file_reader::load_binary_blob("shaders/obj_pixel_shader_normals.cso");
		std::vector<uint8_t> ops_blob = tools::file_reader::load_binary_blob("shaders/obj_pixel_shader.cso");
		std::vector<uint8_t> pss_blob = tools::file_reader::load_binary_blob("shaders/ps_skybox.cso");

		// vertex shaders creation
		HRESULT hr = device->CreateVertexShader(svs_blob.data(), svs_blob.size(), NULL, &vertexShader[VERTEX_SHADER::DEFAULT]);
		assert(!FAILED(hr) && "failed to create simple vertex shader");

		hr = device->CreateVertexShader(ovs_blob.data(), ovs_blob.size(), NULL, &vertexShader[VERTEX_SHADER::OBJECT]);
		assert(!FAILED(hr) && "failed to create object vertex shader");

		hr = device->CreateVertexShader(vss_blob.data(), vss_blob.size(), NULL, &vertexShader[VERTEX_SHADER::SKYBOX]);
		assert(!FAILED(hr) && "failed to create skybox vertex shader");

		// pixel shaders creation
		hr = device->CreatePixelShader(sps_blob.data(), sps_blob.size(), NULL, &pixelShader[PIXEL_SHADER::DEFAULT]);
		assert(!FAILED(hr) && "failed to create simple pixel shader");

		hr = device->CreatePixelShader(onps_blob.data(), onps_blob.size(), NULL, &pixelShader[PIXEL_SHADER::OBJ_NRM]);
		assert(!FAILED(hr) && "failed to create object normal pixel shader");

		hr = device->CreatePixelShader(ops_blob.data(), ops_blob.size(), NULL, &pixelShader[PIXEL_SHADER::OBJ_TEXTURE]);
		assert(!FAILED(hr) && "failed to create object texture pixel shader");

		hr = device->CreatePixelShader(pss_blob.data(), pss_blob.size(), NULL, &pixelShader[PIXEL_SHADER::SKYBOX]);
		assert(!FAILED(hr) && "failed to create skybox texture pixel shader");

		// input layouts

		// simple colored vertex
		D3D11_INPUT_ELEMENT_DESC idesc[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		hr = device->CreateInputLayout(idesc, ARRAYSIZE(idesc), svs_blob.data(), svs_blob.size(), &inputLayout[INPUT_LAYOUT::DEFAULT]);
		assert(!FAILED(hr) && "failed to create simple input layout");

		// object vertex
		D3D11_INPUT_ELEMENT_DESC oidesc[] = {
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		hr = device->CreateInputLayout(oidesc, ARRAYSIZE(oidesc), ovs_blob.data(), ovs_blob.size(), &inputLayout[INPUT_LAYOUT::OBJECT]);
		assert(!FAILED(hr) && "failed to create OBJECT input layout");

	}

	void pipeline::createVertexBuffers() {
		HRESULT hr;
		// simple vertex buffer
		CD3D11_BUFFER_DESC desc = CD3D11_BUFFER_DESC(
			static_cast<UINT>(sizeof(ColoredVertex) * debug_lines::getLineVertCapacity()),
			D3D11_BIND_VERTEX_BUFFER);

		D3D11_SUBRESOURCE_DATA srd = { 0 };
		srd.pSysMem = debug_lines::getLineVerts();

		hr = device->CreateBuffer(&desc, &srd, &vertexBuffer[VERTEX_BUFFER::COLORED_LINES]);
		assert(!FAILED(hr) && "failed to create simple vertex buffer");

		// object vertex buffer
		CD3D11_BUFFER_DESC desc2 = CD3D11_BUFFER_DESC(
			static_cast<UINT>(sizeof(ObjVertex) * 40000),
			D3D11_BIND_VERTEX_BUFFER);

		vBuffer.reserve(40000); // allocating buffer memory
		D3D11_SUBRESOURCE_DATA srd2 = { 0 };
		srd2.pSysMem = vBuffer.data();

		hr = device->CreateBuffer(&desc2, &srd2, &vertexBuffer[VERTEX_BUFFER::OBJ_40000]);
		assert(!FAILED(hr) && "failed to create obj vertex buffer");

		// index buffer
		CD3D11_BUFFER_DESC idesc = CD3D11_BUFFER_DESC(
			sizeof(uint32_t) * 40000,
			D3D11_BIND_INDEX_BUFFER);

		iBuffer.reserve(40000); // allocating buffer memory
		D3D11_SUBRESOURCE_DATA isrd = { 0 };
		isrd.pSysMem = iBuffer.data();


		hr = device->CreateBuffer(&idesc, &isrd, &indexBuffer[INDEX_BUFFER::OBJ_40000]);
		assert(!FAILED(hr) && "failed to create index buffer");
	}

	void pipeline::createConstantBuffers() {
		// Object buffer
		D3D11_BUFFER_DESC obj_cb{ 0 };

		obj_cb.Usage = D3D11_USAGE_DEFAULT;
		obj_cb.ByteWidth = sizeof(Object_cb);
		obj_cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		obj_cb.CPUAccessFlags = 0;

		HRESULT hr = device->CreateBuffer(&obj_cb, NULL, &constantBuffer[CONSTANT_BUFFER::OBJECT_CB]);
		assert(!FAILED(hr) && "failed to create constant buffer: Object_cb");
		
		// Frame buffer
		D3D11_BUFFER_DESC frm_cb{ 0 };

		frm_cb.Usage = D3D11_USAGE_DEFAULT;
		frm_cb.ByteWidth = sizeof(Frame_cb);
		frm_cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		frm_cb.CPUAccessFlags = 0;

		hr = device->CreateBuffer(&frm_cb, NULL, &constantBuffer[CONSTANT_BUFFER::FRAME_CB]);
		assert(!FAILED(hr) && "failed to create constant buffer: Frame_cb");

		// Window buffer
		D3D11_BUFFER_DESC wnd_cb{ 0 };

		wnd_cb.Usage = D3D11_USAGE_DEFAULT;
		wnd_cb.ByteWidth = sizeof(Window_cb);
		wnd_cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		wnd_cb.CPUAccessFlags = 0;

		hr = device->CreateBuffer(&wnd_cb, NULL, &constantBuffer[CONSTANT_BUFFER::WINDOW_CB]);
		assert(!FAILED(hr) && "failed to create constant buffer: Window_cb");

		// Scene buffer
		D3D11_BUFFER_DESC snd_cb{ 0 };

		snd_cb.Usage = D3D11_USAGE_DEFAULT;
		snd_cb.ByteWidth = sizeof(Scene_cb);
		snd_cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		snd_cb.CPUAccessFlags = 0;

		hr = device->CreateBuffer(&snd_cb, NULL, &constantBuffer[CONSTANT_BUFFER::SCENE_CB]);
		assert(!FAILED(hr) && "failed to create constant buffer: Scene_cb");
	}

	void pipeline::setRenderTargetView() {
		static const float black[] = { 0.25f, 0.25f, 0.25f, 1.f };

		context->OMSetDepthStencilState(depthStencilState[DEPTH_STENCIL_STATE::DEFAULT], 1);
		context->OMSetRenderTargets(1, &renderTarget[RENDER_TARGET_VIEW::DEFAULT], depthStencilView[DEPTH_STENCIL_VIEW::DEFAULT]);
		
		context->ClearRenderTargetView(renderTarget[RENDER_TARGET_VIEW::DEFAULT], black);
		context->ClearDepthStencilView(depthStencilView[DEPTH_STENCIL_VIEW::DEFAULT], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		context->RSSetState(rasterState[RASTER_STATE::DEFAULT]);
		context->RSSetViewports(1, &viewPort[VIEWPORT::DEFAULT]);
		
	}

	void pipeline::createSResourceView(const wchar_t* filename, const uint32_t index) {
		HRESULT hr = DirectX::CreateDDSTextureFromFile(device, filename, nullptr, &sResourceView[index]);

		if (FAILED(hr)) {
			throw std::runtime_error("could not load texture file! at: " + index);
		}
	}

	void pipeline::createDebugTexture() {
		const uint32_t pixel = 0xffDBD9CB;//0xffff7798; // a b g r 

		D3D11_SUBRESOURCE_DATA initData = { &pixel, sizeof(uint32_t), 0 };

		const uint32_t pixel2 = 0xff0000ff; // red

		D3D11_SUBRESOURCE_DATA initData2 = { &pixel2, sizeof(uint32_t), 0 };

		const uint32_t pixel3 = 0xff00ff00; // green

		D3D11_SUBRESOURCE_DATA initData3 = { &pixel3, sizeof(uint32_t), 0 };

		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = desc.Height = desc.MipLevels = desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

		ID3D11Texture2D* texture; // don't forget to release
		ID3D11Texture2D* texture2;
		ID3D11Texture2D* texture3;

		HRESULT hr = device->CreateTexture2D(&desc, &initData, &texture);
		if (FAILED(hr)) {
			throw std::runtime_error("could not make debug texture!");
		}

		hr = device->CreateTexture2D(&desc, &initData2, &texture2);
		if (FAILED(hr)) {
			throw std::runtime_error("could not make debug texture!");
		}

		hr = device->CreateTexture2D(&desc, &initData3, &texture3);
		if (FAILED(hr)) {
			throw std::runtime_error("could not make debug texture!");
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		hr = device->CreateShaderResourceView(texture, &srvDesc, &sResourceView[SUBRESOURCE_VIEW::DEBUG]);

		if (FAILED(hr)) {
			throw std::runtime_error("Create Shader resource view for debug texture failed!");
		}

		hr = device->CreateShaderResourceView(texture2, &srvDesc, &sResourceView[SUBRESOURCE_VIEW::DEBUG_RED]);

		if (FAILED(hr)) {
			throw std::runtime_error("Create Shader resource view for debug texture failed!");
		}

		hr = device->CreateShaderResourceView(texture3, &srvDesc, &sResourceView[SUBRESOURCE_VIEW::DEBUG_GREEN]);

		if (FAILED(hr)) {
			throw std::runtime_error("Create Shader resource view for debug texture failed!");
		}


		safe_release(texture);
		safe_release(texture2);
		safe_release(texture3);
	}

} // namespace dxe