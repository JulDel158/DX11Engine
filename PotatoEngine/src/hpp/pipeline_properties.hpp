#pragma once

namespace dxe {

	struct RENDER_TARGET_VIEW {
		enum { DEFAULT = 0, COUNT };
	};

	struct DEPTH_STENCIL_VIEW {
		enum { DEFAULT = 0, COUNT };
	};

	struct DEPTH_STENCIL_STATE {
		enum { DEFAULT = 0, COUNT };
	};

	struct RASTER_STATE {
		enum { DEFAULT = 0, COUNT };
	};

	struct VERTEX_BUFFER {
		enum { COLORED_LINES = 0, OBJ_40000, PARTICLES, COUNT };
	};

	struct INDEX_BUFFER {
		enum { OBJ_40000 = 0, COUNT };
	};

	struct INPUT_LAYOUT {
		enum { DEFAULT = 0, OBJECT, PARTICLES, COUNT };
	};

	struct VERTEX_SHADER {
		enum { DEFAULT = 0, OBJECT, SKYBOX, PARTICLES, COUNT };
	};

	struct PIXEL_SHADER {
		enum { DEFAULT = 0, OBJ_NRM, OBJ_TEXTURE, SKYBOX, PARTICLES, COUNT };
	};

	struct GEOMETRY_SHADER {
		enum { DEFAULT = 0, COUNT };
	};

	struct CONSTANT_BUFFER {
		enum { OBJECT_CB = 0, FRAME_CB, WINDOW_CB, SCENE_CB, COUNT };
	};

	struct VIEWPORT {
		enum { DEFAULT = 0, COUNT };
	};

	struct SUBRESOURCE_VIEW {
		enum { DEBUG = 0, DEFAULT, SKYBOX, DEBUG_RED, DEBUG_GREEN, COUNT };
	};

	struct SAMPLER_STATE {
		enum { DEFAULT = 0, COUNT };
	};

	struct PIPELINE_PROPERTY_DESC {
		uint8_t renderTarget_V;
		uint8_t depthStencil_V;
		uint8_t depthStencil_S;
		uint8_t rasterState;
		uint8_t vertexBuffer;
		uint8_t indexBuffer;
		uint8_t inputLayout;
		uint8_t vertexShader;
		uint8_t pixelShader;
		uint8_t constantBuffer;
		uint8_t viewport;
		uint8_t sResourceView;
		uint8_t samplerState;
	};

} // namespace dxe