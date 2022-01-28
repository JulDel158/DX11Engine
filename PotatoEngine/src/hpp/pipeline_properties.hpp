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
		enum { COLORED_LINES = 0, COUNT };
	};

	struct INDEX_BUFFER {
		enum { DEFAULT = 0, COUNT };
	};

	struct INPUT_LAYOUT {
		enum { DEFAULT = 0, COUNT };
	};

	struct VERTEX_SHADER {
		enum { DEFAULT = 0, COUNT };
	};

	struct PIXEL_SHADER {
		enum { DEFAULT = 0, COUNT };
	};

	struct CONSTANT_BUFFER {
		enum { OBJECT_CB = 0, FRAME_CB, WINDOW_CB, COUNT };
	};

	struct VIEWPORT {
		enum { DEFAULT = 0, COUNT };
	};

	struct SUBRESOURCE_VIEW {
		enum { DEFAULT = 0, COUNT };
	};

	struct SAMPLER_STATE {
		enum { DEFAULT = 0, COUNT };
	};

} // namespace dxe