#include "Utils.h"

#include <iostream>

using namespace rsm;

bool Utils::readFile(const std::string& filepath, std::ios_base::openmode mode, std::string& str) {
	std::ifstream file(filepath, mode);
	if (file.fail()) {
		LOGE("Failed to read file:");
		LOGE("%s", filepath.c_str());
		return false;
	}

	file.seekg(0, std::ios::end);
	str.reserve((size_t)file.tellg());
	file.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	return true;
}

void Utils::throwError(const std::string& error) {
	LOGE("%s", error.c_str());
	std::cin.get();
	exit(EXIT_FAILURE);
}