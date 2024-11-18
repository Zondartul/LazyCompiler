cmake_minimum_required(VERSION 3.10)

# set the project name
IF(WIN32)
	project(lzir.exe)
ELSE()
	project(lzir)
ENDIF()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE source_files "src/*.cpp")

# add the executable
add_executable(${CMAKE_PROJECT_NAME}
				${source_files})


