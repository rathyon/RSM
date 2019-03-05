#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <fstream>

namespace rsm {
	namespace Utils {
		bool readFile   (const std::string& filepath, std::ios_base::openmode mode, std::string& str);
		void throwError (const std::string& error);
	}
}

#endif // !__UTILS_H__