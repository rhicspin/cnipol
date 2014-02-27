#
# Usage:
# export PATH=/star/u/cmake/bin:$PATH
# mkdir build; cd build
# cmake .. -DCMAKE_BUILD_TYPE=Release
# make
#
# After that you will only need to use make.
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

ENABLE_TESTING()

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
# Put root-helper.so in the same directory as executables.  Normally CMake
# would set correct path for us, but we broke this by setting RPATH.
SET_TARGET_PROPERTIES(
	root-helper-utils-shared PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)

SET(
	CNIPOL_INCLUDE_DIRECTORIES
	${CMAKE_CURRENT_SOURCE_DIR}/inc/
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
	inc/AnaFillExternResult.h
	inc/AnaFillResult.h
	inc/AnaGlobResult.h
	inc/AnaInfo.h
	inc/AnaMeasResult.h
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
	inc/MAsymPmtHists.h
	inc/MAsymRateHists.h
	inc/MAsymRoot.h
	inc/MAsymRunHists.h
	inc/MAsymSingleFillHists.h
	inc/MeasInfo.h
	inc/RunQA.h
	inc/Target.h
	online/include/rpoldata.h
	)

ROOT_GENERATE_DICTIONARY(
	Dict
	${LINKDEF_DEPENDENCIES}
	LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h
	)

ADD_LIBRARY(
	cnipol
	SHARED
	src/AlphaCalibrator.cxx
	src/AnaEvent.cxx
	src/AnaEventId.cxx
	src/AnaFillExternResult.cxx
	src/AnaFillResult.cxx
	src/AnaGlobResult.cxx
	src/AnaInfo.cxx
	src/AnaMeasResult.cxx
	src/AsymAnaInfo.cxx
	src/AsymCalculator.cxx
	src/AsymCommon.cxx
	src/AsymDb.cxx
	src/AsymDbFile.cxx
	src/AsymDbSql.cxx
	src/AsymErrorDetector.cxx
	src/AsymOnline.cxx
	src/AsymProcess.cxx
	src/AsymRecover.cxx
	src/AsymRoot.cxx
	src/BeamBunch.cxx
	src/CachingLogReader.cxx
	src/Calibrator.cxx
	src/ChannelCalib.cxx
	src/ChannelData.cxx
	src/ChannelEvent.cxx
	src/ChannelEventId.cxx
	src/CnipolAlphaHists.cxx
	src/CnipolAsymHists.cxx
	src/CnipolCalibHists.cxx
	src/CnipolHists.cxx
	src/CnipolKinemaCleanHists.cxx
	src/CnipolKinematHists.cxx
	src/CnipolPmtHists.cxx
	src/CnipolPreprocHists.cxx
	src/CnipolProfileHists.cxx
	src/CnipolProfileHists.cxx
	src/CnipolPulserHists.cxx
	src/CnipolRawExtendedHists.cxx
	src/CnipolRawHists.cxx
	src/CnipolRunHists.cxx
	src/CnipolScalerHists.cxx
	src/CnipolSpinStudyHists.cxx
	src/CnipolTargetHists.cxx
	src/DbEntry.cxx
	src/DeadLayerCalibrator.cxx
	src/DeadLayerCalibratorEDepend.cxx
	src/DrawObjContainer.cxx
	src/EventConfig.cxx
	src/Kinema.cxx
	src/MAlphaAnaInfo.cxx
	src/MAsymAnaInfo.cxx
	src/MAsymFillHists.cxx
	src/MAsymPmtHists.cxx
	src/MAsymRateHists.cxx
	src/MAsymRoot.cxx
	src/MAsymRunHists.cxx
	src/MAsymSingleFillHists.cxx
	src/MeasInfo.cxx
	src/MseFillPolar.cxx
	src/MseFillPolarNew.cxx
	src/MseFillProfile.cxx
	src/MseFillProfileNew.cxx
	src/MseMeasInfo.cxx
	src/MseRunPeriod.cxx
	src/RawDataReader.cxx
	src/RunConfig.cxx
	src/RunQA.cxx
	src/SshLogReader.cxx
	src/Target.cxx
	src/TargetInfo.cxx
	src/TargetUId.cxx
	src/revision-export.cxx
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
	src/malpha.cxx
	)

TARGET_LINK_LIBRARIES(
	malpha
	cnipol
	)

ADD_EXECUTABLE(
	asym
	src/asym.cxx
	)

TARGET_LINK_LIBRARIES(
	asym
	cnipol
	)

ADD_EXECUTABLE(
	masym
	src/masym.cxx
	)

TARGET_LINK_LIBRARIES(
	masym
	cnipol
	)

ADD_EXECUTABLE(
	spinflipper
	src/spinflipper.cxx
	)

TARGET_LINK_LIBRARIES(
	spinflipper
	cnipol
	)

ADD_EXECUTABLE(
	asym_test
	tests/asym.cxx
	tests/common.cxx
        tests/asym_main.cxx
	)

TARGET_LINK_LIBRARIES(
	asym_test
	cnipol
	)

ADD_TEST(
	NAME asym_test
	COMMAND $<TARGET_FILE:asym_test>
	)

ADD_EXECUTABLE(
	RawDataReader_test
	tests/RawDataReader.cxx
	tests/common.cxx
	)

TARGET_LINK_LIBRARIES(
	RawDataReader_test
	cnipol
	)

ADD_TEST(
	NAME RawDataReader_test
	COMMAND $<TARGET_FILE:RawDataReader_test>
	)

IF(FORCE_32BIT)
	SET_TARGET_PROPERTIES(cnipol PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	SET_TARGET_PROPERTIES(malpha PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	SET_TARGET_PROPERTIES(asym PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	SET_TARGET_PROPERTIES(masym PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	SET_TARGET_PROPERTIES(spinflipper PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	SET_TARGET_PROPERTIES(asym_test PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	SET_TARGET_PROPERTIES(RawDataReader_test PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
ENDIF()
ADD_DEFINITIONS("-Wall -Wextra -Wno-unused-parameter -DCMAKE_BUILD")
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	ADD_DEFINITIONS("-Wno-overloaded-virtual")
ENDIF()

ADD_DEPENDENCIES(cnipol revision_id)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
ADD_CUSTOM_TARGET(revision_id
        ${PROJECT_SOURCE_DIR}/get_revision_id.sh ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/revision-id.h
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        VERBATIM
        )
