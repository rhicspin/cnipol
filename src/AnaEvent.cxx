#include "AnaEvent.h"

ClassImp(AnaEvent)


/** Default constructor. */
AnaEvent::AnaEvent() : TObject(), fEventId(), fChannels()
{
};


/** Default destructor. */
AnaEvent::~AnaEvent()
{
};


float AnaEvent::GetEnergy(EventConfig *ec, ChannelDataPair &ch)
{
   UChar_t chId = ch.first;
   return ec->fCalibrator->fChannelCalibs[chId].fAmAmp.fCoef * fChannels[chId].fAmpltd;
}


float AnaEvent::GetEnergyIntegral(EventConfig *ec, ChannelDataPair &ch)
{
   //int Integ = ch.second.fIntgrl;// << (2 + ec->fConfigInfo->data.CSR.split.iDiv);
	//float amp_int = (Integ - ec->data.chan[st].A0) / cfginfo->data.chan[st].A1;
	//float amp_int = (Integ - 10.) / 100.;

   //return ec->fConfigInfo->data.chan[ch.first].ecoef * amp_int;
   //return ec->fConfigInfo->data.chan[ch.first].ecoef * Integ;
   UChar_t chId = ch.first;
   return ec->fCalibrator->fChannelCalibs[chId].fAmInt.fCoef * fChannels[chId].fIntgrl;
}


float AnaEvent::GetTime(EventConfig *ec, ChannelDataPair &ch)
{
   //return (ec->fConfigInfo->data.WFDTUnit/2.) * (ch.second.fTdc + ec->fRandom->Rndm() - 0.5);
   return WFD_TIME_UNIT_HALF * (ch.second.fTdc + ec->fRandom->Rndm() - 0.5);
}


/**
 *
 */
void AnaEvent::Print(const Option_t* opt) const
{
   printf("AnaEvent:\n");

   ChannelDataMap::const_iterator mi;
   ChannelDataMap::const_iterator mb = fChannels.begin();
   ChannelDataMap::const_iterator me = fChannels.end();

   for (mi=mb; mi!=me; mi++) {
		mi->second.Print();
      printf("\n");
   }
}
