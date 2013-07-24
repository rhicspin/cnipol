// This LinkDef is only used by CMake!

#ifdef __CINT__

// This is LinkDef file used for CMake build

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// inc
#pragma link C++ class AlphaCalibrator+;
#pragma link C++ class AnaEvent+;
#pragma link C++ class AnaEventId+;
#pragma link C++ class AnaFillExternResult;
#pragma link C++ class AnaFillResult;
#pragma link C++ class AnaGlobResult;
#pragma link C++ class AnaInfo+;
#pragma link C++ class AnaMeasResult+;
#pragma link C++ class AsymAnaInfo+;
#pragma link C++ class BeamBunch+;
#pragma link C++ class CalibCoefSet+;
#pragma link C++ class Calibrator+;
#pragma link C++ class ChannelCalib+;
#pragma link C++ class ChannelData+;
#pragma link C++ class ChannelEvent+;
#pragma link C++ class ChannelEventId+;
#pragma link C++ class CnipolAlphaHists+;
#pragma link C++ class CnipolAsymHists+;
#pragma link C++ class CnipolCalibHists+;
#pragma link C++ class CnipolHists+;
#pragma link C++ class CnipolKinemaCleanHists+;
#pragma link C++ class CnipolKinematHists+;
#pragma link C++ class CnipolPmtHists+;
#pragma link C++ class CnipolPreprocHists+;
#pragma link C++ class CnipolProfileHists+;
#pragma link C++ class CnipolPulserHists+;
#pragma link C++ class CnipolRawExtendedHists+;
#pragma link C++ class CnipolRawHists+;
#pragma link C++ class CnipolRunHists+;
#pragma link C++ class CnipolScalerHists+;
#pragma link C++ class CnipolSpinStudyHists+;
#pragma link C++ class CnipolTargetHists+;
#pragma link C++ class DeadLayerCalibrator+;
#pragma link C++ class DeadLayerCalibratorEDepend+;
#pragma link C++ class DrawObjContainer+;
#pragma link C++ class EventConfig+;
#pragma link C++ class MAlphaAnaInfo+;
#pragma link C++ class MAsymAnaInfo+;
#pragma link C++ class MAsymFillHists+;
#pragma link C++ class MAsymSingleFillHists+;
#pragma link C++ class RunQA+;
#pragma link C++ class Target+;
#pragma link C++ class TargetUId+;
#pragma link C++ class MAsymPmtHists+;
#pragma link C++ class MAsymRateHists+;
#pragma link C++ class MAsymRoot+;
#pragma link C++ class MAsymRunHists+;

// online/include/rpoldata.h;
#pragma link C++ class RecordTargetParams-;

// src
#pragma link C++ class AnaFillExternResult+;
#pragma link C++ class AnaFillResult+;
#pragma link C++ class AnaGlobResult+;
#pragma link C++ class AnaMeasResult+;
#pragma link C++ class AsymCalculator+;
#pragma link C++ class MeasInfo+;

// ~dmirnov/rootmacros/utils
#pragma link C++ class ValErrPair+;

#endif
