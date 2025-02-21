cmake_minimum_required(VERSION 3.31.5)

project(mkfat VERSION 1.0.0
		DESCRIPTION "a basic MS-DOS disk image formatter"
		LANGUAGES CXX)

file(GLOB mkfat_src src/*.cpp)
file(GLOB mkfat_inc include/*.h)

add_executable(mkfat ${mkfat_src} ${mkfat_inc})

# we use std::filesystem, so we need c++17
target_compile_features(mkfat PUBLIC cxx_std_17)
target_include_directories(mkfat PRIVATE include)
