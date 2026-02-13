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

file(GLOB_RECURSE dice_server_lib ${CMAKE_CURRENT_SOURCE_DIR}/lib/*)
file(GLOB_RECURSE dice_server_inc ${CMAKE_CURRENT_SOURCE_DIR}/inc/*)
file(GLOB_RECURSE dice_server_app ${CMAKE_CURRENT_SOURCE_DIR}/app/*)

if(NOT TARGET ae3f::libdice)
	ae2f_CoreLibFetchX(ae3f dice master)
endif()

add_executable(dice-server
	${dice_server_app}
	${dice_server_inc}
	${dice_server_lib}
	)
ae2f_UltraErrWERROR(dice-server PRIVATE)

target_link_libraries(dice-server ae3f::rolladie)
target_include_directories(dice-server PRIVATE inc)
