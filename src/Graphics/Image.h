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

	class Image {
	public:
		Image();
		~Image();

		void loadFromFile(const std::string& filepath, ImageType type);
		void loadFromMemory(const char* data, ImageType type);
		void freeImage();

		const int width() const;
		const int height() const;
		const int channels() const;
		const ImageType type() const;
		unsigned char* data() const;

	private:
		int _width;
		int _height;
		int _channels;
		ImageType _type;
		unsigned char* _data;
	};
}



#endif