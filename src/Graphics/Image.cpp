#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

#include "..\ext\stb_image\stb_image.h"

using namespace rsm;

Image::Image() { }

Image::~Image() {
}

void Image::loadFromFile(const std::string& filepath, ImageType type) {
	stbi_set_flip_vertically_on_load(true);
	_data = stbi_load(filepath.c_str(), &_width, &_height, &_channels, 0);

	// giving "Corrupt JPEG on PNG loading... wtf
	// Android: STB ERROR: Unable to open file
	//LOGE("STB ERROR: %s\n", stbi_failure_reason());

	_type = type;
}

void Image::loadFromMemory(const char* data, size_t length, ImageType type) {
	stbi_set_flip_vertically_on_load(true);
	_data = stbi_load_from_memory((unsigned char*)data, length, &_width, &_height, &_channels, 0);

    //LOGE("STB ERROR: %s\n", stbi_failure_reason());
	_type = type;
}

// Remember to delete the image after generating the textures!
void Image::freeImage() {
	stbi_image_free(_data);
}

const int Image::width() const {
	return _width;
}

const int Image::height() const {
	return _height;
}

const int Image::channels() const {
	return _channels;
}

const ImageType Image::type() const {
	return _type;
}

unsigned char* Image::data() const {
	return _data;
}