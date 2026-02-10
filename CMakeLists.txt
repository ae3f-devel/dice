cmake_minimum_required(VERSION 3.10)
project(dice)

enable_testing()

# Set this source directory as the tree root for project.
set(ae2f_ProjRoot ${CMAKE_CURRENT_SOURCE_DIR} CACHE STRING "Tree root for project")
set(ae2f_submod  .submod CACHE STRING "Relative path to submodule (subdirectory)")
set(ae2f_CoreScript_Wh ${ae2f_ProjRoot}/${ae2f_submod}/ae2f/CoreScript)

macro(ae3f_dice_addsubdir p)
	if(NOT TARGET ae3f::${p})
		add_subdirectory(${p})
	endif()
endmacro()

macro(dice_addsubdir p)
	if(NOT TARGET ${p})
		add_subdirectory(${p})
	endif()
endmacro()

ae3f_dice_addsubdir(libdice)
ae3f_dice_addsubdir(dasm)
ae3f_dice_addsubdir(rolladie)
dice_addsubdir(dice-server)
