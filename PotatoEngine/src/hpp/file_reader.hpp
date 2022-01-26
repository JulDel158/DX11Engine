#pragma once
#include <vector>

// this is a tool class to read various types of files and retrive their data into the required formats

namespace tools {

	class file_reader {
	public:
		static std::vector<uint8_t> load_binary_blob(const char* path);
	};

} // namespace tools