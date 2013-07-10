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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)
PROJECT(CNIPOL)

# Make CMake obey LD_RUN_PATH environment variable
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
SET(CMAKE_INSTALL_RPATH "${LIBRARY_OUTPUT_PATH}:${CMAKE_CURRENT_BINARY_DIR}:$ENV{LD_RUN_PATH}")
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})
SET_PROPERTY(GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS OFF)

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/Site-Config.cmake)

FIND_PACKAGE(MySQL)
FIND_PACKAGE(MySQLPP)
FIND_PACKAGE(ROOT)

SET(
	CNIPOL_INCLUDE_DIRECTORIES
	${CMAKE_CURRENT_SOURCE_DIR}/inc/
	${CMAKE_CURRENT_SOURCE_DIR}/src/
	${CMAKE_CURRENT_SOURCE_DIR}/online/include/
	/eic/u/dsmirnov/rootmacros/
	)

INCLUDE_DIRECTORIES(
	${ROOT_INCLUDE_DIR}
	${MYSQL_INCLUDE_DIR}
	${MYSQL++_INCLUDE_DIR}
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
	online/include/rpoldata.h
	src/AnaFillExternResult.h
	src/AnaFillResult.h
	src/AnaGlobResult.h
	src/AnaMeasResult.h
	src/MeasInfo.h
#	/eic/u/dsmirnov/rootmacros/utils/ValErrPair.h
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
	ana/MAsymSingleFillHists.cxx
	ana/RunConfig.cxx
	ana/RunQA.cxx
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
	~dsmirnov/rootmacros/utils/ValErrPair.cxx
	~dsmirnov/rootmacros/utils/utils.C
	Dict.cxx
	)

TARGET_LINK_LIBRARIES(
	cnipol
	${MYSQL_LIBRARY}
	${MYSQL++_LIBRARIES}
	${ROOT_LIBRARIES}
	)

# I know it's tempting to change this, but please don't
SET_TARGET_PROPERTIES(cnipol PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")

ADD_EXECUTABLE(
	malpha
	src/malpha.cc
	)

TARGET_LINK_LIBRARIES(
	malpha
	cnipol
	)

SET_TARGET_PROPERTIES(malpha PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")

ADD_EXECUTABLE(
	asym
	src/asym.cc
	ana/SshLogReader.cxx
	)

TARGET_LINK_LIBRARIES(
	asym
	cnipol
	)

SET_TARGET_PROPERTIES(asym PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")

ADD_DEFINITIONS("-Wall -Wextra -Wno-unused-parameter")
ADD_DEFINITIONS("-DASYM_VERSION=\"not-implemented-yet\"")