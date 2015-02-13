# vim: set noexpandtab:

INCLUDE_DIRECTORIES(
	${OPENCDEV_INCLUDE_DIR}
	)

ROOT_GENERATE_DICTIONARY(
	Dict
	${LINKDEF_DEPENDENCIES}
	LINKDEF ${CMAKE_CURRENT_SOURCE_DIR}/../LinkDef.h
	)

INCLUDE_DIRECTORIES(
	${ROOT_INCLUDE_DIR}
	${MYSQL++_INCLUDE_DIR}
	${SQLITE3_INCLUDE_DIR}
	)

ADD_LIBRARY(
	cnipol
	SHARED
	AlphaCalibrator.cxx
	AnaEvent.cxx
	AnaEventId.cxx
	AnaFillExternResult.cxx
	AnaFillResult.cxx
	AnaGlobResult.cxx
	AnaInfo.cxx
	AnaMeasResult.cxx
	AsymAnaInfo.cxx
	AsymCalculator.cxx
	AsymCommon.cxx
	AsymDb.cxx
	AsymDbFile.cxx
	AsymDbSql.cxx
	AsymErrorDetector.cxx
	AsymOnline.cxx
	AsymRecover.cxx
	AsymRoot.cxx
	BeamBunch.cxx
	BiasCurrentUtil.cxx
	CachingLogReader.cxx
	Calibrator.cxx
	ChannelCalib.cxx
	ChannelData.cxx
	ChannelEvent.cxx
	ChannelEventId.cxx
	CnipolAlphaHists.cxx
	CnipolAsymHists.cxx
	CnipolCalibHists.cxx
	CnipolHists.cxx
	CnipolKinemaCleanHists.cxx
	CnipolKinematHists.cxx
	CnipolPmtHists.cxx
	CnipolPreprocHists.cxx
	CnipolProfileHists.cxx
	CnipolProfileHists.cxx
	CnipolPulserHists.cxx
	CnipolRawExtendedHists.cxx
	CnipolRawHists.cxx
	CnipolRunHists.cxx
	CnipolScalerHists.cxx
	CnipolSpinStudyHists.cxx
	CnipolTargetHists.cxx
	DbEntry.cxx
	DeadLayerCalibrator.cxx
	DeadLayerCalibratorEDepend.cxx
	DrawObjContainer.cxx
	EventConfig.cxx
	MAlphaAnaInfo.cxx
	MAsymAnaInfo.cxx
	MAsymFillHists.cxx
	MAsymPmtHists.cxx
	MAsymRateHists.cxx
	MAsymRoot.cxx
	MAsymRunHists.cxx
	MAsymSingleFillHists.cxx
	MeasInfo.cxx
	MseFillPolar.cxx
	MseFillPolarNew.cxx
	MseFillProfile.cxx
	MseFillProfileNew.cxx
	MseMeasInfo.cxx
	MseRunPeriod.cxx
	RawDataReader.cxx
	RunConfig.cxx
	RunQA.cxx
	SshLogReader.cxx
	Target.cxx
	TargetInfo.cxx
	TargetUId.cxx
	revision-export.cxx
	Dict.cxx
	cnipol_toolkit.cxx
	)

TARGET_LINK_LIBRARIES(
	cnipol
	${MYSQL++_LIBRARIES}
	${ROOT_LIBRARIES}
	${SQLITE3_LIBRARIES}
	${OPENCDEV_LIBRARIES}
	root-helper-utils-shared
	)

ADD_EXECUTABLE(asym		asym.cxx)
TARGET_LINK_LIBRARIES(asym	cnipol)

ADD_EXECUTABLE(masym		masym.cxx)
TARGET_LINK_LIBRARIES(masym	cnipol)

ADD_EXECUTABLE(malpha		malpha.cxx)
TARGET_LINK_LIBRARIES(malpha	cnipol)

ADD_EXECUTABLE(mbias		mbias.cxx)
TARGET_LINK_LIBRARIES(mbias	cnipol ${MYSQL++_LIBRARIES})

ADD_EXECUTABLE(mbias-meas		mbias-meas.cxx)
TARGET_LINK_LIBRARIES(mbias-meas	cnipol ${MYSQL++_LIBRARIES})

ADD_EXECUTABLE(spinflipper	spinflipper.cxx)
TARGET_LINK_LIBRARIES(spinflipper	cnipol)

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
