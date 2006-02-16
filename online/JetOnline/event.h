* event information
      integer*4 iSpill, iAGScycle, iAGSint,
     +          iStrip, iEpoch, iAdc, iTdc, iB, iPol, iGCC
      real*4 P, E, Tof, Intg
      integer*4 wfddata(90)
      common /atdata/ iSpill, iAGScycle, iAGSint, P, iGCC, 
     +                iStrip, iEpoch, E, iAdc, Tof, 
     +                iTdc, Intg, iB, iPol,
     +                wfddata

* "calculated" values
      integer*4 gats(4) 
      integer*4 wave_f,hbk_cont,total_hbk,atyp 
      common /atdata2/ gats,total_hbk,hbk_cont,wave_f,
     +                 atyp
