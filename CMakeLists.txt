cmake_minimum_required(VERSION 3.31.5)

project(mkfat VERSION 1.0.0
		DESCRIPTION "a basic MS-DOS disk image formatter"
		LANGUAGES CXX)

# generate compile_commands.json for IDE integration
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=1)

file(GLOB mkfat_source src/*.cpp)
file(GLOB mkfat_include include/*.h)

add_executable(mkfat ${mkfat_source} ${mkfat_include})

# we use std::filesystem, so we need c++17
target_compile_features(mkfat PUBLIC cxx_std_17)
target_include_directories(mkfat PRIVATE include)

# make sure all errors are known
target_compile_options(mkfat PRIVATE "-Wall" "-Wextra" "-Wpedantic")
