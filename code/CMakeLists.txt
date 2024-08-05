cmake_minimum_required(VERSION 3.10)

# set the project name
IF(WIN32)
	project(lazycomp.exe)
ELSE()
	project(lazycomp)
ENDIF()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)
set(CMAKE_C_FLAGS "-g -fmax-errors=1 -std=c11 -Wall -Wextra -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -fno-stack-protector")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
set(CMAKE_EXE_LINKER_FLAGS "")
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lasan")
#======================
#variables
#important:
# paths:
#  CMAKE_BINARY_DIR - path to the top of the build tree
#  CMAKE_SOURCE_DIR - path to the top of the source tree
# tools:
#  CMAKE_GENERATOR - generator used to build files - not modified by code,
#		     only selected through -G option or an env. variable



# set build folder
#set(CMAKE_BINARY_DIR build)

#figure out how to set C standard version, this one is for C++
#set(CMAKE_CXX_STANDARD 17)

include_directories(
	include
	include/codegen
	include/codegen/types
	include/main
	include/preproc
	include/semantic
	include/semantic/types
	include/semantic/template
	include/util
	generated
)

set(path_yacc_tab_c ${CMAKE_CURRENT_SOURCE_DIR}/generated/yaccin.tab.c)
set(path_yacc_tab_h ${CMAKE_CURRENT_SOURCE_DIR}/generated/yaccin.tab.h)

add_custom_command(OUTPUT ${path_yacc_tab_c} ${path_yacc_tab_h}
				   COMMAND bison -d -Wall -ogenerated/yaccin.tab.c input/yaccin.y #-Wcounterexamples --update
				   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				   DEPENDS input/yaccin.y)

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/generated/lex.yy.c
				   COMMAND flex -ogenerated/lex.yy.c < input/lexin.l
				   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
				   DEPENDS input/lexin.l ${path_yacc_tab_h})
				   

file(GLOB_RECURSE source_files "include/*.h" "source/*.c")


# add the executable
add_executable(lazycomp 
				generated/yaccin.tab.c
				generated/lex.yy.c 
				${source_files})


