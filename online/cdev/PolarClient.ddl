service Polar
	{
	tags { }
	}

/******************************************/
/*************** WCM **********************/
/******************************************/
class wcm {
	verbs {get, getMetaData, monitorOn, monitorOff}	
	attributes
		{
		fillDataM Polar {server=PolarServer};
		fillDataM:valueAndTime Polar {server=PolarServer};
		wcmBeamM Polar {server=PolarServer};		/* Bunched beam intencity */
		wcmBeamM:valueAndTime Polar {server=PolarServer};
		}
	}
wcm : bo2-wcm3
      yi2-wcm3;


/*****************************************/
/********cav197Voltage********************/
/*****************************************/
class cav {
        verbs {get, getMetaData, monitorOn, monitorOff}
        attributes
                {
                probeMagInVoltsScaledM Polar {server=PolarServer};
                probeMagInVoltsScaledM:valueAndTime Polar {server=PolarServer};
                }
        }
cav : cavTuneLoop.4a-rf-b197-1.3
      cavTuneLoop.4a-rf-y197-1.3;


/******************************************/
/*************** specMan ******************/
/******************************************/
class specMan {
	verbs {get, getMetaData, monitorOn, monitorOff}	
	attributes
		{	
		beamEnergyM Polar {server=PolarServer};
		beamEnergyM:valueAndTime Polar {server=PolarServer};
      		fillNumberM Polar {server=PolarServer};
		fillNumberM:valueAndTime Polar {server=PolarServer};
		ringStateS Polar {server=PolarServer};
		ringStateS:valueAndTime Polar {server=PolarServer};
                ionSpeciesS Polar {server=PolarServer};
                ionSpeciesS:valueAndTime Polar {server=PolarServer};
                gammaM Polar {server=PolarServer};
                gammaM:valueAndTime Polar {server=PolarServer};
                stoneTypeS Polar {server=PolarServer};				/* n/i */
                stoneTypeS:valueAndTime Polar {server=PolarServer};		
                momentumSpreadS Polar {server=PolarServer};			/* n/i */
                momentumSpreadS:valueAndTime Polar {server=PolarServer};	
                chromaticityVerticalS Polar {server=PolarServer};
                chromaticityVerticalS:valueAndTime Polar {server=PolarServer};
                chromaticityHorizontalS Polar {server=PolarServer};
                chromaticityHorizontalS:valueAndTime Polar {server=PolarServer};
                timeOfFillStartS Polar {server=PolarServer};
                timeOfFillStartS:valueAndTime Polar {server=PolarServer};
                timeOfLuminosityStartS Polar {server=PolarServer};
                timeOfLuminosityStartS:valueAndTime Polar {server=PolarServer};
		}	
	}
specMan : ringSpec.blue
       	  ringSpec.yellow;

/******************************************/
/*************** PatternMan****************/
/******************************************/
class buckets {
	verbs {get, getMetaData, monitorOn, monitorOff}	
	attributes
		{	
                bunchLengthS Polar {server=PolarServer};
                intendedFillPatternS Polar {server=PolarServer};
                measuredFillPatternM Polar {server=PolarServer};
                bunchOneRelativePhaseS Polar {server=PolarServer};
                polarizationFillPatternS Polar {server=PolarServer};
                fillPatternThresholdS Polar {server=PolarServer};
                measuredPolarizationUpS Polar {server=PolarServer};
                measuredPolarizationDownS Polar {server=PolarServer};
                spinFlipSuccessRateS Polar {server=PolarServer};
                spinFlipSuccessThresholdS Polar {server=PolarServer};
                spinFlipAttemptsM Polar {server=PolarServer};
                spinFlipSuccessesM Polar {server=PolarServer};
                totalIntendedM Polar {server=PolarServer};
                filledBucketsS Polar {server=PolarServer};
                patternRotationM Polar {server=PolarServer};
                bunchLengthS:valueAndTime Polar {server=PolarServer};
                intendedFillPatternS:valueAndTime Polar {server=PolarServer};
                measuredFillPatternM:valueAndTime Polar {server=PolarServer};
                bunchOneRelativePhaseS:valueAndTime Polar {server=PolarServer};
                polarizationFillPatternS:valueAndTime Polar {server=PolarServer};
                fillPatternThresholdS:valueAndTime Polar {server=PolarServer};
                measuredPolarizationUpS:valueAndTime Polar {server=PolarServer};
                measuredPolarizationDownS:valueAndTime Polar {server=PolarServer};
                spinFlipSuccessRateS:valueAndTime Polar {server=PolarServer};
                spinFlipSuccessThresholdS:valueAndTime Polar {server=PolarServer};
                spinFlipAttemptsM:valueAndTime Polar {server=PolarServer};
                spinFlipSuccessesM:valueAndTime Polar {server=PolarServer};
                totalIntendedM:valueAndTime Polar {server=PolarServer};
                filledBucketsS:valueAndTime Polar {server=PolarServer};
                patternRotationM:valueAndTime Polar {server=PolarServer};
		}	
	}
buckets : buckets.blue
       	  buckets.yellow;

/******************************************/
/*************** polhtarget ***************/
/******************************************/
class polhtarget {
	verbs {get, getMetaData, set, monitorOn, monitorOff}	
	attributes
		{
		movingM Polar {server=PolarServer};
		movingM:valueAndTime Polar {server=PolarServer};
		positionEncM Polar {server=PolarServer};
		positionEncM:valueAndTime Polar {server=PolarServer};
      		statusM Polar {server=PolarServer};
		statusM:valueAndTime Polar {server=PolarServer};
      		positionS Polar {server=PolarServer};
		positionS:valueAndTime Polar {server=PolarServer};
		initLinS Polar {server=PolarServer};
		initLinS:valueAndTime Polar {server=PolarServer};
		initRotS Polar {server=PolarServer};
		initRotS:valueAndTime Polar {server=PolarServer};
		limitLinM Polar {server=PolarServer};
		limitLinM:valueAndTime Polar {server=PolarServer};
		limitRotM Polar {server=PolarServer};
		limitRotM:valueAndTime Polar {server=PolarServer};
		}
	}
polhtarget : pol.b-htarget
	     pol.y-htarget;
	     
polhtarget : pol2.b-htarget
	     pol2.y-htarget;

/******************************************/
/*************** polvtarget ***************/
/******************************************/
class polvtarget : polhtarget {
}
polvtarget : pol.b-vtarget
	     pol.y-vtarget;
	     
polvtarget : pol2.b-vtarget
	     pol2.y-vtarget;

/******************************************/
/*************** polarMux ***************/
/******************************************/
class polarMux {
    verbs {get, getMetaData, monitorOn, monitorOff}
    attributes {
        polNumM Polar {server=PolarServer};
        polNumM:valueAndTime Polar {server=PolarServer};
        switchAllInputsS Polar {server=PolarServer};
        switchAllInputsS:valueAndTime Polar {server=PolarServer};
        commStatusM Polar {server=PolarServer};
        commStatusM:valueAndTime Polar {server=PolarServer};
    }
}
polarMux : polarMux.RHIC.blu
           polarMux.RHIC.yel;

/******************************************/
/************* polarimeterBlue ************/
/******************************************/
class polarimeterBlue {
	verbs {get, getMetaData, set, monitorOn, monitorOff}
	attributes
		{
		runIdS Polar {server=PolarServer};
		runIdS:valueAndTime Polar {server=PolarServer};
		startTimeS Polar {server=PolarServer};
		startTimeS:valueAndTime Polar {server=PolarServer};
		stopTimeS Polar {server=PolarServer};
		stopTimeS:valueAndTime Polar {server=PolarServer};
      		daqVersionS Polar {server=PolarServer};
      		daqVersionS:valueAndTime Polar {server=PolarServer};
      		cutIdS Polar {server=PolarServer};
      		cutIdS:valueAndTime Polar {server=PolarServer};
      		targetIdS Polar {server=PolarServer};
      		targetIdS:valueAndTime Polar {server=PolarServer};
      		encoderPositionS Polar {server=PolarServer};
      		encoderPositionS:valueAndTime Polar {server=PolarServer};
      		statusS Polar {server=PolarServer};
      		statusS:valueAndTime Polar {server=PolarServer};
      		statusStringS Polar {server=PolarServer};
      		statusStringS:valueAndTime Polar {server=PolarServer};
      		totalCountsS Polar {server=PolarServer};
      		totalCountsS:valueAndTime Polar {server=PolarServer};
      		upCountsS Polar {server=PolarServer};
      		upCountsS:valueAndTime Polar {server=PolarServer};
      		downCountsS Polar {server=PolarServer};
      		downCountsS:valueAndTime Polar {server=PolarServer};
      		unpolCountsS Polar {server=PolarServer};
      		unpolCountsS:valueAndTime Polar {server=PolarServer};
      		countsUpLeftS Polar {server=PolarServer};
      		countsUpLeftS:valueAndTime Polar {server=PolarServer};
      		countsLeftS Polar {server=PolarServer};
      		countsLeftS:valueAndTime Polar {server=PolarServer};
      		countsDownLeftS Polar {server=PolarServer};
      		countsDownLeftS:valueAndTime Polar {server=PolarServer};
      		countsDownRightS Polar {server=PolarServer};
      		countsDownRightS:valueAndTime Polar {server=PolarServer};
      		countsRightS Polar {server=PolarServer};
      		countsRightS:valueAndTime Polar {server=PolarServer};
      		countsUpRightS Polar {server=PolarServer};
      		countsUpRightS:valueAndTime Polar {server=PolarServer};
      		avgAsymXS Polar {server=PolarServer};
      		avgAsymXS:valueAndTime Polar {server=PolarServer};
      		avgAsymX45S Polar {server=PolarServer};
      		avgAsymX45S:valueAndTime Polar {server=PolarServer};
      		avgAsymX90S Polar {server=PolarServer};
      		avgAsymX90S:valueAndTime Polar {server=PolarServer};
      		avgAsymYS Polar {server=PolarServer};
      		avgAsymYS:valueAndTime Polar {server=PolarServer};
      		avgAsymErrorXS Polar {server=PolarServer};
      		avgAsymErrorXS:valueAndTime Polar {server=PolarServer};
      		avgAsymErrorX45S Polar {server=PolarServer};
      		avgAsymErrorX45S:valueAndTime Polar {server=PolarServer};
      		avgAsymErrorX90S Polar {server=PolarServer};
      		avgAsymErrorX90S:valueAndTime Polar {server=PolarServer};
      		avgAsymErrorYS Polar {server=PolarServer};
      		avgAsymErrorYS:valueAndTime Polar {server=PolarServer};
      		bunchAsymXS Polar {server=PolarServer};
      		bunchAsymXS:valueAndTime Polar {server=PolarServer};
     		bunchAsymYS Polar {server=PolarServer};
     		bunchAsymYS:valueAndTime Polar {server=PolarServer};
      		bunchAsymErrorXS Polar {server=PolarServer};
      		bunchAsymErrorXS:valueAndTime Polar {server=PolarServer};
      		bunchAsymErrorYS Polar {server=PolarServer};
      		bunchAsymErrorYS:valueAndTime Polar {server=PolarServer};
      		analyzingPowerS Polar {server=PolarServer};
      		analyzingPowerS:valueAndTime Polar {server=PolarServer};
      		analyzingPowerErrorS Polar {server=PolarServer};
      		analyzingPowerErrorS:valueAndTime Polar {server=PolarServer};
		beamEnergyS Polar {server=PolarServer};
		beamEnergyS:valueAndTime Polar {server=PolarServer};
		numberEventsS Polar {server=PolarServer};
		numberEventsS:valueAndTime Polar {server=PolarServer};
		maxTimeS Polar {server=PolarServer};
		maxTimeS:valueAndTime Polar {server=PolarServer};
		numberEventsToDoS Polar {server=PolarServer};
		numberEventsToDoS:valueAndTime Polar {server=PolarServer};
		maxTimeToRunS Polar {server=PolarServer};
		maxTimeToRunS:valueAndTime Polar {server=PolarServer};
		dataAcquisitionS Polar {server=PolarServer};
		dataAcquisitionS:valueAndTime Polar {server=PolarServer};
		dataAcquisitionType Polar {server=PolarServer};
		dataAcquisitionType:valueAndTime Polar {server=PolarServer};
		progressS Polar {server=PolarServer};
		countRateS Polar {server=PolarServer};
		inBeamVertS Polar {server=PolarServer};
		inBeamHorzS Polar {server=PolarServer};
		outBeamVertS Polar {server=PolarServer};
		outBeamHorzS Polar {server=PolarServer};
		encPosProfileS Polar {server=PolarServer};
		encPosProfileS:valueAndTime Polar {server=PolarServer};
                plotData Polar {server=PolarServer};
                plotData:valueAndTime Polar {server=PolarServer};
                emitRunIdS Polar {server=PolarServer};
                emitRunIdS:valueAndTime Polar {server=PolarServer};
                emitPeakS Polar {server=PolarServer};
                emitPeakS:valueAndTime Polar {server=PolarServer};
                emitWidthS Polar {server=PolarServer};
                emitWidthS:valueAndTime Polar {server=PolarServer};
                emitRelWidthS Polar {server=PolarServer};
                emitRelWidthS:valueAndTime Polar {server=PolarServer};	
                emitPeaktoEvtS Polar {server=PolarServer};
                emitPeaktoEvtS:valueAndTime Polar {server=PolarServer};
                emitPeakBunchS Polar {server=PolarServer};
                emitPeakBunchS:valueAndTime Polar {server=PolarServer};
                emitWidthBunchS Polar {server=PolarServer};
                emitWidthBunchS:valueAndTime Polar {server=PolarServer};
                emitRelWidthBunchS Polar {server=PolarServer};
                emitRelWidthBunchS:valueAndTime Polar {server=PolarServer};
                emitPeaktoEvtBunchS Polar {server=PolarServer};
                emitPeaktoEvtBunchS:valueAndTime Polar {server=PolarServer};
                emitPlot Polar {server=PolarServer};
                emitPlot:valueAndTime Polar {server=PolarServer};
                rampMeasTimeS Polar {server=PolarServer};
                rampMeasTimeS:valueAndTime Polar {server=PolarServer};
                asymXS Polar {server=PolarServer};
                asymXS:valueAndTime Polar {server=PolarServer};
                asymErrorXS Polar {server=PolarServer};
                asymErrorXS:valueAndTime Polar {server=PolarServer};
                asymX45S Polar {server=PolarServer};
                asymX45S:valueAndTime Polar {server=PolarServer};
                asymErrorX45S Polar {server=PolarServer};
                asymErrorX45S:valueAndTime Polar {server=PolarServer};
                asymX90S Polar {server=PolarServer};
                asymX90S:valueAndTime Polar {server=PolarServer};
                asymErrorX90S Polar {server=PolarServer};
                asymErrorX90S:valueAndTime Polar {server=PolarServer};
                asymXTsS Polar {server=PolarServer};
                asymXTsS:valueAndTime Polar {server=PolarServer};
                polarizationM Polar {server=PolarServer};
                polarizationM:valueAndTime Polar {server=PolarServer};
		}
	}
polarimeterBlue : polarimeter.blu
		  polarimeter.blu1
		  polarimeter.blu2;

/******************************************/
/************ polarimeterYellow ***********/
/******************************************/
class polarimeterYellow : polarimeterBlue 
{
}
polarimeterYellow : polarimeter.yel
		    polarimeter.yel1
		    polarimeter.yel2;

/******************************************/
/************* fecPolSystem ***************/
/******************************************/
class fecPolarSystem 
        {
        verbs {get, getMetaData, monitorOn, monitorOff}
        attributes
                {
                currentTime Polar {server=PolarServer};
                }
        }
fecPolarSystem : fecSystem.12a-polar1;

/******************************************/
/******** AgsPolarimeterSystem ************/
/******************************************/
class AgsPolarimeter
	{
	verbs {get, set, monitorOn, monitorOff, getMetaData}
	attributes
		{
		runIdS Polar {server=PolarServer};
		startTimeS Polar {server=PolarServer};
		stopTimeS Polar {server=PolarServer};
		daqVersionS Polar {server=PolarServer};
		targetIdS Polar {server=PolarServer};
		encoderPositionS Polar {server=PolarServer};
		statusS Polar {server=PolarServer};
		statusStringS Polar {server=PolarServer};
		numberOfBunchS Polar {server=PolarServer};
		numberOfBucketS Polar {server=PolarServer};
		bunchPatternS Polar {server=PolarServer};
		startSpillS Polar {server=PolarServer};
		totalSpillS Polar {server=PolarServer};
		totalNeventS Polar {server=PolarServer};
		beamEnergyAtFlattopS Polar {server=PolarServer};
		momentumBinS Polar {server=PolarServer};
		leftMostBinEdgeS Polar {server=PolarServer};
		binWidthS Polar {server=PolarServer};
		asymAtRampS Polar {server=PolarServer}; 
		asymAtRampErrorS Polar {server=PolarServer};
		analyzingPowerS Polar {server=PolarServer};
		analyzingPowerErrorS Polar {server=PolarServer};
		epsilonPhysicsS Polar {server=PolarServer};
		epsilonAcceptanceS Polar {server=PolarServer};
		epsilonLuminosityS Polar {server=PolarServer};
		ratioLuminosityS Polar {server=PolarServer};
		epsilonError Polar {server=PolarServer};
                plotData Polar {server=PolarServer};
		}
	}

AgsPolarimeter : agsPolarimeter;


/******************************************/
/**************** stepper  ****************/
/******************************************/
class stepper
        {
	verbs {get, getMetaData, monitorOn, monitorOff}
	attributes
	            {
	            absolutePosition Polar {server=PolarServer};
	            }
	}
stepper : stepper.12a-hjet.A.U;

/******************************************/
/**************** rbpm ********************/
/******************************************/
class rbpm
        {
	verbs {get, getMetaData, monitorOn, monitorOff}
	attributes
	        {
	        avgOrbPositionM Polar {server=PolarServer};
	        }
	}
rbpm :  rbpm.b-g11-bhx
        rbpm.b-g12-bhx
	rbpm.b-g12-bvx
	rbpm.b-g11-bvx
	rbpm.y-g11-bhx
	rbpm.y-g12-bhx
	rbpm.y-g11-bvx
	rbpm.y-g12-bvx;

/*************************************/
/************* hjet.test *************/
/*************************************/
class iobitText
    {
    verbs {get, getMetaData, monitorOn, monitorOff}
	attributes
	        {
	        outBitsText Polar {server=PolarServer};
	        }
	}
iobitText : iobitText.12a-hjet.11
            iobitText.12a-hjet.15;

/*************************************/
/***** polarControlDefaults **********/
/*************************************/
class polarControlDefaults {
    verbs {get, getMetaData, monitorOn, monitorOff}
    attributes {
	rampInterval Polar {server=PolarServer};
    }
}

polarControlDefaults :  polarControlDefaults.blu1
			polarControlDefaults.blu2
			polarControlDefaults.yel1
			polarControlDefaults.yel2;


class BsTrigChanYellow
    {    
    verbs {get, set, getMetaData, monitorOn, monitorOff}
    attributes
            {
        positionDelay Polar {server=PolarServer};
        linkStatus Polar {server=PolarServer};
        mode Polar {server=PolarServer};
        pulseWidthMode Polar {server=PolarServer};
        fineDelayCounter Polar {server=PolarServer};
        pulseWidthCounter Polar {server=PolarServer};
        buckets Polar {server=PolarServer};
        addBucket Polar {server=PolarServer};
        removeBucket Polar {server=PolarServer};
        nBuckets Polar {server=PolarServer};
        bucketOffset Polar {server=PolarServer};
        bucketOffsetS Polar {server=PolarServer};
            }
    }

BsTrigChanYellow : bsTrigChan.12a-polar1.C.1
                   bsTrigChan.12a-polar1.C.2
                   bsTrigChan.12a-polar1.C.3
                   bsTrigChan.12a-polar1.C.4
                   bsTrigChan.12a-polar1.C.5
                   bsTrigChan.12a-polar1.C.6
                   bsTrigChan.12a-polar1.C.7
                   bsTrigChan.12a-polar1.C.8
                   bsTrigMod.12a-polar1.C;

class BsTrigChanBlue
    {    
    verbs {get, set, getMetaData, monitorOn, monitorOff}
    attributes
            {
        positionDelay Polar {server=PolarServer};
        linkStatus Polar {server=PolarServer};
        mode Polar {server=PolarServer};
        pulseWidthMode Polar {server=PolarServer};
        fineDelayCounter Polar {server=PolarServer};
        pulseWidthCounter Polar {server=PolarServer};
        buckets Polar {server=PolarServer};
        addBucket Polar {server=PolarServer};
        removeBucket Polar {server=PolarServer};
        nBuckets Polar {server=PolarServer};
        bucketOffset Polar {server=PolarServer};
        bucketOffsetS Polar {server=PolarServer};
            }
    }

BsTrigChanBlue : bsTrigChan.12a-polar1.D.1
                   bsTrigChan.12a-polar1.D.2
                   bsTrigChan.12a-polar1.D.3
                   bsTrigChan.12a-polar1.D.4
                   bsTrigChan.12a-polar1.D.5
                   bsTrigChan.12a-polar1.D.6
                   bsTrigChan.12a-polar1.D.7
                   bsTrigChan.12a-polar1.D.8
                   bsTrigMod.12a-polar1.D;

/*************************************/
/************* simple.test ***********/
/*************************************/
class simpleTest {
        verbs {get, set, monitorOn, monitorOff}
        attributes
                {
                fecName Polar {server=PolarServer};
                description Polar {server=PolarServer};
                constructTime Polar {server=PolarServer};
                version Polar {server=PolarServer};
                className Polar {server=PolarServer};
                commandBuffer Polar {server=PolarServer};
                commandBufferLength Polar {server=PolarServer};
                commandC Polar {server=PolarServer};
                menuS Polar {server=PolarServer};
                charS Polar {server=PolarServer};
                ucharS Polar {server=PolarServer};
                shortS Polar {server=PolarServer};
                ushortS Polar {server=PolarServer};
                longS Polar {server=PolarServer};
                ulongS Polar {server=PolarServer};
                floatS Polar {server=PolarServer};
                doubleS Polar {server=PolarServer};
                stringS Polar {server=PolarServer};
                charArrayS Polar {server=PolarServer};
                ucharArrayS Polar {server=PolarServer};
                shortArrayS Polar {server=PolarServer};
                sinM Polar {server=PolarServer};
                degM Polar {server=PolarServer};
                }
        }

simpleTest : simple.test;
