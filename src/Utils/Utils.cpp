#include "Utils.h"

#include <iostream>

using namespace rsm;

bool Utils::readFile(const std::string& filepath, std::ios_base::openmode mode, std::string& str) {
	std::ifstream file(filepath, mode);
	if (file.fail()) {
		perror(filepath.c_str());
		return false;
	}

	file.seekg(0, std::ios::end);
	str.reserve((size_t)file.tellg());
	file.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
}

void Utils::throwError(const std::string& error) {
	std::cerr << error << std::endl;
	std::cin.get();
	exit(EXIT_FAILURE);
}