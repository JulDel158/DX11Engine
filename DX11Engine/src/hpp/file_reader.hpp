#pragma once
#include <vector>
#include <functional>

// this is a tool class to read various types of files and retrive their data into the required formats

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
	};

} // namespace tools