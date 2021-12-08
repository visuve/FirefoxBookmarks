#include "lz4.h"

#include <iostream>
#include <fstream>
#include <filesystem>

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage:" << std::endl;
		std::cerr << argv[0] << " <input-file> <output-file>" << std::endl;
		std::cerr << "E.g. <bookmarks.json.lz4> <bookmarks.json>" << std::endl;
		return -1;
	}

	std::filesystem::path input_path(argv[1]);
	std::filesystem::path output_path(argv[2]);

	int file_size = static_cast<int>(std::filesystem::file_size(input_path)) - 12;

	if (file_size <= 0)
	{
		std::cerr << "The input file appears empty!" << std::endl;
		return -2;
	}

	std::ifstream input_file(input_path, std::ifstream::binary);

	std::string header(8, 0);
	input_file.read(header.data(), 8);

	if (!std::string_view(header).starts_with("mozLz40"))
	{
		std::cerr << "The input file does not have a Mozilla header!" << std::endl;
		return -3;
	}

	uint32_t space_required = 0;
	input_file.read(reinterpret_cast<char*>(&space_required), 4);

	if (!space_required)
	{
		std::cerr << "Failed to process bytes needed to uncompress!" << std::endl;
		return -4;
	}

	std::string compressed(file_size, '\0');
	input_file.read(compressed.data(), file_size);

	std::string uncompressed(space_required, '\0');

	int bytes_decoded = LZ4_decompress_safe_partial(
		compressed.data(),
		uncompressed.data(),
		file_size,
		space_required,
		space_required);

	if (bytes_decoded <= 0)
	{
		std::cerr << "LZ4_decompress_safe_partial failed!" << std::endl;
		return -5;
	}

	std::ofstream output_file(output_path);
	output_file.write(uncompressed.data(), static_cast<size_t>(bytes_decoded));

	return 0;
}
