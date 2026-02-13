# ===============================================

cmake_minimum_required(VERSION 3.10)
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

project(libdice VERSION 0)

file(GLOB_RECURSE libdice_c ${CMAKE_CURRENT_SOURCE_DIR}/lib/*)
file(GLOB_RECURSE libdice_h ${CMAKE_CURRENT_SOURCE_DIR}/inc/*)

if(NOT DEFINED LIBDICE_LOOKUP_SECTION_WORD_LEN)
	set(LIBDICE_LOOKUP_SECTION_WORD_LEN 32)
endif()

ae2f_CoreLibFetchX(ae2f Core	main)
ae2f_CoreLibFetchX(ae2f Sys	main)

ae2f_CoreLibTentConfigCustom(libdice STATIC inc ae3f ${ae2f_ProjRoot}/cfg/libdice.cmake.in
	${libdice_c} ${libdice_h}
	)

configure_file(
	${PROJECT_SOURCE_DIR}/inc/libdice/lookup.h.in
	${PROJECT_SOURCE_DIR}/inc/libdice/.lookup.h
	)

target_link_libraries(ae3f-libdice ae2f::Sys)
ae2f_UltraErrWERROR(ae3f-libdice PRIVATE)
ae2f_CoreTestTentVerbose(ae3f-libdice test)
