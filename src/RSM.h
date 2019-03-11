#ifndef __RSM_RSM_H__
#define __RSM_RSM_H__

// Global includes
#include <cstdint>
#include <type_traits>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

#define make_sref std::make_shared

#define LOG_TAG "RSM"

#ifdef __ANDROID__
#include <android/log.h>

#define LOG(str) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, (str).c_str())
#define LOGE(str) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, (str).c_str())

#else

#define LOG(str) std::cout << str << std::endl
#define LOGE(str) std::cerr << str << std::endl

#endif

namespace rsm {

	// These are relics from the PBR project, ignore it for the most part for now

	// Integer types
	typedef std::uint8_t  uint8;
	typedef std::uint16_t uint16;
	typedef std::uint32_t uint32;
	typedef std::uint64_t uint64;

	typedef std::int8_t  int8;
	typedef std::int16_t int16;
	typedef std::int32_t int32;
	typedef std::int64_t int64;

	template<typename T>
	using sref = std::shared_ptr<T>;
}

#endif