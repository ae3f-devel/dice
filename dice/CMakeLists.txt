# ===============================================

enable_testing()

# Set this source directory as the tree root for project.
set(ae2f_ProjRoot ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "Tree root for project")
set(ae2f_submod  .submod CACHE STRING "Relative path to submodule (subdirectory)")
set(ae2f_CoreScript_Wh ${ae2f_ProjRoot}/${ae2f_submod}/ae2f/CoreScript)

# Ensure CoreScript helper is available (fetch if missing)
if(NOT EXISTS ${ae2f_CoreScript_Wh})
	execute_process(COMMAND git clone https://codeberg.org/ae2f/CoreScript ${ae2f_CoreScript_Wh})
endif()

if(NOT TARGET ae2f::CoreScript)
	add_subdirectory(${ae2f_CoreScript_Wh} ${ae2f_submod}/ae2f/CoreScript)
endif()

# ===============================================

cmake_minimum_required(VERSION 3.10)
project(dice)

# sources
file(GLOB_RECURSE dice_app ${CMAKE_CURRENT_SOURCE_DIR}/app/*.c)
file(GLOB_RECURSE dice_inc ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h)

# executable
add_executable(dice
	${dice_app}
	${dice_inc}
)

ae2f_UltraErrWERROR(dice PRIVATE)

target_link_libraries(dice ae3f::rolladie)
target_include_directories(dice PRIVATE inc)