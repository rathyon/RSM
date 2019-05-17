# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/../../../src/)

file (GLOB SOURCE_FILES
        ${SOURCE_DIR}RSM.h
        ${SOURCE_DIR}*
        ${SOURCE_DIR}App/*
        ${SOURCE_DIR}Core/*
        ${SOURCE_DIR}Graphics/*
        ${SOURCE_DIR}GUI/*
        ${SOURCE_DIR}Lights/*
        ${SOURCE_DIR}Materials/*
        ${SOURCE_DIR}Math/*
        ${SOURCE_DIR}Utils/*
        ${SOURCE_DIR}App/Android/*
        ${SOURCE_DIR}../ext/tinyobj/*
        ${SOURCE_DIR}../ext/stb_image/*)

add_library( # Sets the name of the library.
        AndroidRSM-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${SOURCE_FILES})

target_include_directories(AndroidRSM-lib PUBLIC ${SOURCE_DIR})

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        AndroidRSM-lib

        android
        EGL
        GLESv3
        log)