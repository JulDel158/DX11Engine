#include "../hpp/file_reader.hpp"

// std
#include <fstream>

namespace tools {

	std::vector<uint8_t> file_reader::load_binary_blob(const char* path) {
		std::vector<uint8_t> blob;

		std::fstream file{ path, std::ios_base::in | std::ios_base::binary };

		if (file.is_open()) {

			file.seekg(0, std::ios_base::end);
			blob.resize(file.tellg());
			file.seekg(0, std::ios_base::beg);

			file.read((char*)blob.data(), blob.size());

			file.close();
		}

		return blob;
	}
} // namespace tools