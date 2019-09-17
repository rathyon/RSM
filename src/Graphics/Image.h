#ifndef __RSM_IMAGE_H__
#define __RSM_IMAGE_H__

/*
	Class used exclusively for reading image files from memory
	OpenGL related operations are made in Texture class
*/

#include <iostream>
#include "RSM.h"

namespace rsm {

	enum ImageType {
		IMG_1D   = 0,
		IMG_2D   = 1,
		IMG_3D   = 2,
		IMG_CUBE = 3
	};

	/* ASTC header declaration. */
	typedef struct
	{
		unsigned char magic[4];
		unsigned char blockdim_x;
		unsigned char blockdim_y;
		unsigned char blockdim_z;
		unsigned char xsize[3];
		unsigned char ysize[3];
		unsigned char zsize[3];
	} astc_header;

	class Image {
	public:
		Image();
		~Image();

		void loadFromFile(const std::string& filepath, ImageType type);
		void loadFromMemory(const char* data, size_t length, ImageType type);
		void freeImage();

		const int width() const;
		const int height() const;
		const int channels() const;
		const ImageType type() const;
		unsigned char* data() const;
		const int size() const;

	private:
		int _width;
		int _height;
		int _channels;
		ImageType _type;
		unsigned char* _data;

		//ASTC additions
		int _size; //size in bytes
	};
}



#endif