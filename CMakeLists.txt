# vim: set noexpandtab:

#
# Usage:
# export PATH=/star/u/cmake/bin:$PATH
# mkdir build; cd build
# cmake .. -DCMAKE_BUILD_TYPE=Release
# make
#
# After that you will only need to use make.
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)
PROJECT(CNIPOL)

SET(CMAKE_CXX_STANDARD 11)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})
OPTION(FORCE_32BIT "should be OFF" OFF) # ON value is not supported

ADD_DEFINITIONS("-DCNIPOL_ROOT_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}\"")

FIND_PACKAGE(Sqlite3)
FIND_PACKAGE(ROOT)

IF(${ROOT_VERSION} VERSION_GREATER "6.0")
	MESSAGE(WARNING You are trying to build against ROOT6. cnipol in some
	      places uses ROOT2 style streamers that are broken in ROOT6.  So
	      far problems observed with RecordMachineParams,
	      RecordTargetParams, AnaInfo.)
ENDIF()

EXECUTE_PROCESS(
	COMMAND git submodule init
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	)
EXECUTE_PROCESS(
	COMMAND git submodule update
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	)
ADD_SUBDIRECTORY(contrib/root-helper EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY(contrib/opencdev EXCLUDE_FROM_ALL)
ADD_SUBDIRECTORY(contrib/mysqlpp EXCLUDE_FROM_ALL)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

SET(
	CNIPOL_INCLUDE_DIRECTORIES
	${CMAKE_CURRENT_SOURCE_DIR}/inc/
	${CMAKE_CURRENT_SOURCE_DIR}/contrib/polarim-online/include/
	${ROOT_HELPER_DIR}
	)

INCLUDE_DIRECTORIES(
	${CNIPOL_INCLUDE_DIRECTORIES}
	)

ADD_SUBDIRECTORY(inc)
ADD_SUBDIRECTORY(src)
