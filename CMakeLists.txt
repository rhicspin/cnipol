#
# Usage:
# export PATH=/star/u/cmake/bin:$PATH
# mkdir build; cd build
# cmake .. -DCMAKE_BUILD_TYPE=Debug
# make
#
# After that you will only need to use make.
#

#
# Due to this bug https://bugzilla.redhat.com/show_bug.cgi?id=493929
# Releas build type (i.e. optimizatiations) won't work unless you use
# newer version of gcc:
#
# export PATH=/afs/rhic.bnl.gov/rcassoft/x8664_sl5/gcc471/bin:$PATH
# export LD_RUN_PATH=/afs/rhic.bnl.gov/rcassoft/x8664_sl5/gcc471/lib:$LD_RUN_PATH
# export LD_LIBRARY_PATH=/afs/rhic.bnl.gov/rcassoft/x8664_sl5/gcc471/lib:$LD_LIBRARY_PATH
# CXX=`which gcc` cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
# make
#

#
# To compile 64-bit version do:
#
# mkdir build64
# cmake .. -DFORCE_32BIT=OFF
# make
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)
PROJECT(CNIPOL)

# Make CMake obey LD_RUN_PATH environment variable
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
SET(CMAKE_INSTALL_RPATH "${LIBRARY_OUTPUT_PATH}:${CMAKE_CURRENT_BINARY_DIR}:$ENV{LD_RUN_PATH}:$ENV{LD_LIBRARY_PATH}")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})
OPTION(FORCE_32BIT "compile 32-bit application" ON)
IF(FORCE_32BIT)
	SET_PROPERTY(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)
ENDIF()

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/Site-Config.cmake)

FIND_PACKAGE(MySQL)
FIND_PACKAGE(MySQLPP)
FIND_PACKAGE(SQLite)
FIND_PACKAGE(ROOT)
EXECUTE_PROCESS(
	COMMAND git submodule init
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	)
EXECUTE_PROCESS(
	COMMAND git submodule update
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	)
ADD_SUBDIRECTORY(contrib/root-helper)

SET(
	CNIPOL_INCLUDE_DIRECTORIES
	${CMAKE_CURRENT_SOURCE_DIR}/inc/
	${CMAKE_CURRENT_SOURCE_DIR}/src/
	${CMAKE_CURRENT_SOURCE_DIR}/online/include/
	${ROOT_HELPER_DIR}
	)

INCLUDE_DIRECTORIES(
	${ROOT_INCLUDE_DIR}
	${MYSQL_INCLUDE_DIR}
	${MYSQL++_INCLUDE_DIR}
	${SQLITE_INCLUDE_DIR}
	${CNIPOL_INCLUDE_DIRECTORIES}
	)

SET(
	LINKDEF_DEPENDENCIES
	inc/AlphaCalibrator.h
	inc/AnaEvent.h
	inc/AnaEventId.h
	inc/AnaInfo.h
	inc/AsymAnaInfo.h
	inc/BeamBunch.h
	inc/CalibCoefSet.h
	inc/Calibrator.h
	inc/ChannelCalib.h
	inc/ChannelData.h
	inc/ChannelEventId.h
	inc/CnipolAlphaHists.h
	inc/CnipolAsymHists.h
	inc/CnipolCalibHists.h
	inc/CnipolHists.h
	inc/CnipolKinemaCleanHists.h
	inc/CnipolKinematHists.h
	inc/CnipolPmtHists.h
	inc/CnipolPreprocHists.h
	inc/CnipolProfileHists.h
	inc/CnipolPulserHists.h
	inc/CnipolRawExtendedHists.h
	inc/CnipolRawHists.h
	inc/CnipolRunHists.h
	inc/CnipolScalerHists.h
	inc/CnipolSpinStudyHists.h
	inc/CnipolTargetHists.h
	inc/DeadLayerCalibrator.h
	inc/DeadLayerCalibratorEDepend.h
	inc/EventConfig.h
	inc/MAlphaAnaInfo.h
	inc/MAsymFillHists.h
	inc/MAsymSingleFillHists.h
	inc/RunQA.h
	inc/Target.h
	inc/MAsymPmtHists.h
	inc/MAsymRateHists.h
	inc/MAsymRoot.h
	inc/MAsymRunHists.h
	online/include/rpoldata.h
	src/AnaFillExternResult.h
	src/AnaFillResult.h
	src/AnaGlobResult.h
	src/AnaMeasResult.h
	src/MeasInfo.h
	)

ROOT_GENERATE_DICTIONARY(
	"${LINKDEF_DEPENDENCIES}"
	${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h
	Dict.cxx
	"${CNIPOL_INCLUDE_DIRECTORIES}"
	)

ADD_LIBRARY(
	cnipol
	SHARED
	ana/AlphaCalibrator.cxx
	ana/AnaEvent.cxx
	ana/AnaEventId.cxx
	ana/AnaInfo.cxx
	ana/AsymAnaInfo.cxx
	ana/BeamBunch.cxx
	ana/CachingLogReader.cxx
	ana/Calibrator.cxx
	ana/ChannelCalib.cxx
	ana/ChannelData.cxx
	ana/ChannelEvent.cxx
	ana/ChannelEventId.cxx
	ana/CnipolAlphaHists.cxx
	ana/CnipolAsymHists.cxx
	ana/CnipolCalibHists.cxx
	ana/CnipolHists.cxx
	ana/CnipolKinemaCleanHists.cxx
	ana/CnipolKinematHists.cxx
	ana/CnipolPmtHists.cxx
	ana/CnipolPreprocHists.cxx
	ana/CnipolProfileHists.cxx
	ana/CnipolProfileHists.cxx
	ana/CnipolPulserHists.cxx
	ana/CnipolRawExtendedHists.cxx
	ana/CnipolRawHists.cxx
	ana/CnipolRunHists.cxx
	ana/CnipolScalerHists.cxx
	ana/CnipolSpinStudyHists.cxx
	ana/CnipolTargetHists.cxx
	ana/DeadLayerCalibrator.cxx
	ana/DeadLayerCalibratorEDepend.cxx
	ana/DrawObjContainer.cxx
	ana/EventConfig.cxx
	ana/MAlphaAnaInfo.cxx
	ana/MAsymAnaInfo.cxx
	ana/MAsymFillHists.cxx
	ana/MAsymPmtHists.cxx
	ana/MAsymRateHists.cxx
	ana/MAsymRoot.cxx
	ana/MAsymRunHists.cxx
	ana/MAsymSingleFillHists.cxx
	ana/RunConfig.cxx
	ana/RunQA.cxx
	ana/SshLogReader.cxx
	ana/Target.cxx
	ana/TargetUId.cxx
	src/AnaFillExternResult.cc
	src/AnaFillResult.cc
	src/AnaGlobResult.cc
	src/AnaMeasResult.cc
	src/AsymCalculator.cc
	src/AsymCommon.cc
	src/AsymDb.cc
	src/AsymDbFile.cc
	src/AsymDbSql.cc
	src/AsymErrorDetector.cc
	src/AsymOnline.cc
	src/AsymProcess.cc
	src/AsymRead.cc
	src/AsymRecover.cc
	src/AsymRoot.cc
	src/DbEntry.cc
	src/Kinema.cc
	src/MeasInfo.cc
	src/MseFillPolar.cc
	src/MseFillPolarNew.cc
	src/MseFillProfile.cc
	src/MseFillProfileNew.cc
	src/MseMeasInfo.cc
	src/MseRunPeriod.cc
	src/TargetInfo.cc
	src/revision-export.cpp
	Dict.cxx
	)

TARGET_LINK_LIBRARIES(
	cnipol
	${MYSQL_LIBRARY}
	${MYSQL++_LIBRARIES}
	${ROOT_LIBRARIES}
	${SQLITE_LIBRARY}
	root-helper-utils-shared
	)

ADD_EXECUTABLE(
	malpha
	src/malpha.cc
	)

TARGET_LINK_LIBRARIES(
	malpha
	cnipol
	)

ADD_EXECUTABLE(
	asym
	src/asym.cc
	)

TARGET_LINK_LIBRARIES(
	asym
	cnipol
	)

ADD_EXECUTABLE(
	masym
	src/masym.cc
	)

TARGET_LINK_LIBRARIES(
	masym
	cnipol
	)

ADD_EXECUTABLE(
	spinflipper
	src/spinflipper.cc
	)

TARGET_LINK_LIBRARIES(
	spinflipper
	cnipol
	)

IF(FORCE_32BIT)
	SET_TARGET_PROPERTIES(cnipol PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	SET_TARGET_PROPERTIES(malpha PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	SET_TARGET_PROPERTIES(asym PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	SET_TARGET_PROPERTIES(masym PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	SET_TARGET_PROPERTIES(spinflipper PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
ENDIF()
ADD_DEFINITIONS("-Wall -Wextra -Wno-unused-parameter -DCMAKE_BUILD")
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	ADD_DEFINITIONS("-Wno-overloaded-virtual")
ENDIF()

ADD_DEPENDENCIES(cnipol revision_id)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
ADD_CUSTOM_TARGET(revision_id
        ${PROJECT_SOURCE_DIR}/get_revision_id.sh ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/revision-id.hpp
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM
        )
