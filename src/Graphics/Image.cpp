#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG

#include "..\ext\stb_image\stb_image.h"

#define MALLOC_CHECK(ptr_type, ptr, size)                                        \
{                                                                                \
    ptr = (ptr_type) malloc(size);                                               \
    if (ptr == NULL)                                                             \
    {                                                                            \
        LOGE("Memory allocation error FILE: %s LINE: %i\n", __FILE__, __LINE__); \
        exit(EXIT_FAILURE);                                                      \
    }                                                                            \
}

using namespace rsm;

Image::Image() { }

Image::~Image() {
}

void Image::loadFromFile(const std::string& filepath, ImageType type) {
	/** /
	stbi_set_flip_vertically_on_load(true);
	_data = stbi_load(filepath.c_str(), &_width, &_height, &_channels, 0);

	// giving "Corrupt JPEG on PNG loading... wtf
	// Android: STB ERROR: Unable to open file
	//LOGE("STB ERROR: %s\n", stbi_failure_reason());

	_type = type;
	/**/

	/**/
	unsigned char* input_data = NULL;

	long				file_size = 0;
	unsigned int		n_bytes_to_read = 0;
	size_t				result = 0;

	//Number of blocks in the x, y and z direction.
	int xblocks = 0;
	int yblocks = 0;
	int zblocks = 0;

	//Number of bytes for each dimension.
	int xsize = 0;
	int ysize = 0;
	int zsize = 0;

	FILE* compressed_data_file = fopen(filepath.c_str(), "rb");

	if (compressed_data_file == NULL)
	{
		LOGE("Could not open a file.\n");
	}

	//Obtain file size.
	fseek(compressed_data_file, 0, SEEK_END);
	file_size = ftell(compressed_data_file);
	rewind(compressed_data_file);

	MALLOC_CHECK(unsigned char*, input_data, sizeof(unsigned char) * file_size);

	result = fread(input_data, 1, file_size, compressed_data_file);

	if (result != file_size)
	{
		LOGE("Reading error [%s] ... FILE: %s LINE: %i\n", filepath.c_str(), __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	astc_header* astc_data_ptr = (astc_header*)input_data;

	xsize = astc_data_ptr->xsize[0] + (astc_data_ptr->xsize[1] << 8) + (astc_data_ptr->xsize[2] << 16);
	ysize = astc_data_ptr->ysize[0] + (astc_data_ptr->ysize[1] << 8) + (astc_data_ptr->ysize[2] << 16);
	zsize = astc_data_ptr->zsize[0] + (astc_data_ptr->zsize[1] << 8) + (astc_data_ptr->zsize[2] << 16);

	xblocks = (xsize + astc_data_ptr->blockdim_x - 1) / astc_data_ptr->blockdim_x;
	yblocks = (ysize + astc_data_ptr->blockdim_y - 1) / astc_data_ptr->blockdim_y;
	zblocks = (zsize + astc_data_ptr->blockdim_z - 1) / astc_data_ptr->blockdim_z;

	n_bytes_to_read = xblocks * yblocks * zblocks << 4;

	_data = input_data;
	_width = xsize;
	_height = ysize;
	_type = type;
	_size = n_bytes_to_read;
	/**/
}

void Image::loadFromMemory(const char* data, size_t length, ImageType type) {
	/** /
	stbi_set_flip_vertically_on_load(true);
	_data = stbi_load_from_memory((unsigned char*)data, length, &_width, &_height, &_channels, 0);

    //LOGE("STB ERROR: %s\n", stbi_failure_reason());
	_type = type;
	/**/

	/**/
	unsigned int		n_bytes_to_read = 0;

	// Number of blocks in the x, y and z direction.
	int xblocks = 0;
	int yblocks = 0;
	int zblocks = 0;

	// Number of bytes for each dimension.
	int xsize = 0;
	int ysize = 0;
	int zsize = 0;

	// Traverse the file structure.
	astc_header* astc_data_ptr = (astc_header*) data;

	//Merge x,y,z-sizes from 3 chars into one integer value.
	xsize = astc_data_ptr->xsize[0] + (astc_data_ptr->xsize[1] << 8) + (astc_data_ptr->xsize[2] << 16);
	ysize = astc_data_ptr->ysize[0] + (astc_data_ptr->ysize[1] << 8) + (astc_data_ptr->ysize[2] << 16);
	zsize = astc_data_ptr->zsize[0] + (astc_data_ptr->zsize[1] << 8) + (astc_data_ptr->zsize[2] << 16);

	// Compute number of blocks in each direction.
	xblocks = (xsize + astc_data_ptr->blockdim_x - 1) / astc_data_ptr->blockdim_x;
	yblocks = (ysize + astc_data_ptr->blockdim_y - 1) / astc_data_ptr->blockdim_y;
	zblocks = (zsize + astc_data_ptr->blockdim_z - 1) / astc_data_ptr->blockdim_z;

	// Each block is encoded on 16 bytes, so calculate total compressed image data size.
	n_bytes_to_read = xblocks * yblocks * zblocks << 4;

	_data = (unsigned char*)data;
	_width = xsize;
	_height = ysize;
	_type = type;
	_size = n_bytes_to_read;
	/**/
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

const int Image::size() const {
	return _size;
}