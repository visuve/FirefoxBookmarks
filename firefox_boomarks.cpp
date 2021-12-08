#include "lz4.h"

#include <iostream>
#include <fstream>
#include <filesystem>

std::string uncompress(std::ifstream& stream, int compressed_size, int uncompressed_size)
{
	std::string compressed(compressed_size, '\0');
	stream.read(compressed.data(), compressed_size);

	std::string uncompressed(uncompressed_size, '\0');

	int bytes_decompressed = LZ4_decompress_safe(
		compressed.data(),
		uncompressed.data(),
		compressed_size,
		uncompressed_size);

	if (bytes_decompressed <= 0)
	{
		return {};
	}

	if (bytes_decompressed < uncompressed_size)
	{
		uncompressed.resize(bytes_decompressed);
	}

	return uncompressed;
}

int uncompressed_size_bytes(std::ifstream& stream)
{
	int result = 0;
	stream.read(reinterpret_cast<char*>(&result), 4);
	return result;
}

bool has_mozilla_lz4_header(std::ifstream& stream)
{
	std::string header(8, 0);
	stream.read(header.data(), 8);
	return header.starts_with("mozLz40");
}

void usage(const std::filesystem::path& executable)
{
	std::cerr << "Usage:" << std::endl;
	std::cerr << executable.string() << " <input-file> <output-file>" << std::endl;
	std::cerr << "Example:" << std::endl;
#ifndef _WIN32
	std::cerr << "./";
#endif
	std::cerr << executable.filename().string() << " bookmarks.json.lz4 bookmarks.json" << std::endl;
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		usage(argv[0]);
		return -1;
	}

	std::filesystem::path input_path(argv[1]);
	std::filesystem::path output_path(argv[2]);

	int compressed_size = static_cast<int>(std::filesystem::file_size(input_path)) - 12;

	if (compressed_size <= 0)
	{
		std::cerr << "The input file appears empty!" << std::endl;
		return -2;
	}

	std::ifstream input_file(input_path, std::ifstream::binary);

	if (!has_mozilla_lz4_header(input_file))
	{
		std::cerr << "The input file does not have a Mozilla header!" << std::endl;
		return -3;
	}

	int uncompressed_size = uncompressed_size_bytes(input_file);

	if (uncompressed_size <= 0)
	{
		std::cerr << "Failed to read bytes needed to uncompress!" << std::endl;
		return -4;
	}

	std::string uncompressed = uncompress(input_file, compressed_size, uncompressed_size);

	if (uncompressed.empty())
	{
		std::cerr << "LZ4_decompress_safe_partial failed!" << std::endl;
		return -5;
	}

	std::ofstream output_file(output_path);

	return output_file.write(uncompressed.data(), uncompressed.size()) ? 0 : -6;
}
