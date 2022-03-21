#pragma once


// std
#include <assert.h>
#include <functional>
#include <fstream>
#include <vector>
#include <string>

// this is a tool class to read various types of files and retrive their data into the required formats

namespace dxe {
	struct Terrain;
}

namespace tools {

	// from: https://stackoverflow.com/a/57595105
	template <typename T, typename... Rest>
	void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
		seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		(hashCombine(seed, rest), ...);
	};

	class file_reader {
	public:
		static std::vector<uint8_t> load_binary_blob(const char* path);

		static void loadBVH(const char* filepath, dxe::Terrain& object);
	};

	void replaceExtension(std::string& filepath, std::string& extension);

	

	void exportBVH(const char* outputFile, const dxe::Terrain& object);

} // namespace tools