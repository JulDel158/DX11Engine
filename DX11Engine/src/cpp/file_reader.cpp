#include "../hpp/file_reader.hpp"
#include "../hpp/object_data.hpp"

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

	void file_reader::loadBVH(const char* filepath, dxe::Terrain& object) {
		std::ifstream file{ filepath, std::ios::in | std::ios::binary };

		assert(file.is_open() && "Failed to open file: " && filepath);

		uint32_t treeSize{ 0 }, triangleCount{ 0 };

		// loading tree data
		file.read((char*)&treeSize, sizeof(uint32_t));
		object.tree.resize(treeSize);
		file.read((char*)object.tree.data(), treeSize * sizeof(dxe::bvh_node));

		// loading triangle data
		file.read((char*)&triangleCount, sizeof(uint32_t));
		object.triangles.resize(triangleCount);
		file.read((char*)object.triangles.data(), triangleCount * sizeof(dxe::Triangle_i));

		file.close();
	}

	void replaceExtension(std::string& filepath, std::string& extension) {
		std::string::size_type i = filepath.rfind('.', filepath.length());

		if (i != std::string::npos) {
			filepath.replace(i + 1, extension.length(), extension);
		}
	}


	void exportBVH(const char* outputFile, const dxe::Terrain& object) {

		std::ofstream file(outputFile, std::ios::trunc | std::ios::binary | std::ios::out);
		assert(file.is_open() && "Failed to open: " && outputFile);

		const uint32_t treeSize = static_cast<uint32_t>(object.tree.size());
		const uint32_t triangleCount = static_cast<uint32_t>(object.triangles.size());

		// tree data
		file.write((const char*)&treeSize, sizeof(uint32_t));
		file.write((const char*)object.tree.data(), sizeof(dxe::bvh_node) * object.tree.size());

		// triangle data
		file.write((const char*)&triangleCount, sizeof(uint32_t));
		file.write((const char*)object.triangles.data(), sizeof(dxe::Triangle_i) * object.triangles.size());

		file.close();
	}

} // namespace tools