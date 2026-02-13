# ===============================================

enable_testing()

# Set this source directory as the tree root for project.
set(ae2f_ProjRoot ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "Tree root for project")
set(ae2f_submod  .submod CACHE STRING "Relative path to submodule (subdirectory)")
set(ae2f_CoreScript_Wh ${ae2f_ProjRoot}/${ae2f_submod}/ae2f/CoreScript)

# Execute git to fetch CoreScript when repository does not exist
if(NOT EXISTS ${ae2f_CoreScript_Wh})
	execute_process(
		COMMAND git clone https://codeberg.org/ae2f/CoreScript
		${ae2f_CoreScript_Wh}
		)
endif()

# add subdirectory
if(NOT TARGET ae2f::CoreScript)
	add_subdirectory(${ae2f_CoreScript_Wh} ${ae2f_submod}/ae2f/CoreScript)
endif()

# ===============================================

cmake_minimum_required(VERSION 3.10)
project(libdice VERSION 0)

file(GLOB_RECURSE asmdice_c ${CMAKE_CURRENT_SOURCE_DIR}/lib/*)
file(GLOB_RECURSE asmdice_h ${CMAKE_CURRENT_SOURCE_DIR}/inc/*)

if(NOT TARGET ae3f::libdice)
	ae2f_CoreLibFetchX(ae3f dice master)
endif()

ae2f_Macro_init(1 30 30)
ae2f_Macro_Lib_V2(
	ae3f rolladie STATIC
	${PROJECT_SOURCE_DIR}/lib/ "*.c"
	${PROJECT_SOURCE_DIR}/src/ "*.def.c"
	${PROJECT_SOURCE_DIR}/inc/ ".auto.h"
	${ae2f_ProjRoot}/cfg/rolladie.cmake.in
	inc ON
	)

target_link_libraries(ae3f-rolladie ae3f::libdice)
ae2f_UltraErrWERROR(ae3f-rolladie PRIVATE)
ae2f_CoreTestTentVerbose(ae3f-rolladie test)
