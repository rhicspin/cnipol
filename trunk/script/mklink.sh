#!/bin/bash
# mklink.sh
# Mar. 8, 2006
# I.Nakagawa
RAW_DATALIST="$DATADIR/raw_data.list";
RUN_YEAR=2009;
ExeStandAlonePC2PC=0;
ExeDoublePC=0;
ExeSimple=0;
ExeCustomDir=0; 
 
#############################################################################
#                                     Help                                  #
#############################################################################
help(){
    echo    " "
    echo    " mklink.sh [-xh][--pc2pc-stand-alone][--double-pc][--pb2pb-2005][--dir <dir>]"
    echo    "    : make symbolic links"
    echo    " "
    echo -e "   --pc2pc-stand-alone    Stand alone pc2pc configulation";
    echo -e "   --double-pc            Double PC configulation";
    echo -e "   --pb2pb-2005           cd to DATADIR, ONLINE/log, hbook and execute ln.sh"
    echo -e "   --dir <dir>            make symbolic links of files in /<dir>/$RUN_YEAR/ directory"
    echo -e "   -h | --help            Show this help"
    echo -e "   -x                     Show example"
    echo    " "
    exit;
}

 
ShowExample(){
 
    echo    " "
    echo    "1. make links to /data dir on pc2pc, /Ydata and /Bdata dir"
    echo    " "
    echo    "    mklink.sh --pc2pc-stand-alone --double-pc"
    echo    " "
    echo    "2. three external disc configulation on Run05 pb2pb"
    echo    " "
    echo    "    mklink.sh --pb2pb-2005"
    echo    " "
    echo    "3. specify directory for /bd2/data/2009"
    echo    " "
    echo    "    mklink.sh --dir bd2/data "
    echo    " "
    exit;

}



#############################################################################
#                                   MakeLinks                               #
#############################################################################
MakeLinks(){

    #create directory if there is no such a directory
    if [ ! -d $DESTDIR ] ; then
	echo -e "Create $DESTDIR";
	mkdir -p $DESTDIR
    fi
    echo $BEAM - $DESTDIR 

    
    #make simbolik link if there is no existing links
    SOURCEDIR=/$BEAM/$RUN_YEAR/$Mode
    for f in `ls $SOURCEDIR | grep $Mode` ; 
      do 
      filename=`basename $f`
      if [ ! -e $DESTDIR/$filename ] ; then
	  echo -e -n "$f\n" /dev/null
	  ln -f -s $SOURCEDIR/$f $DESTDIR
      fi
    done


    # show list of files after making links
    ls --color=tty $DESTDIR 

}


#############################################################################
#                                   DoublePC                                #
#############################################################################

DoublePC(){

    for BEAM in Bdata Ydata ; do 

	DESTDIR=$DATADIR;
	Mode=data;
	MakeLinks;

	DESTDIR=$ONLINEDIR/log;
	Mode=log;
	MakeLinks;

	DESTDIR=$ONLINEDIR/hbook;
	Mode=hbook;
	MakeLinks;

    done

    
}


#############################################################################
#                               StandAlonePC2PC                             #
#############################################################################

StandAlonePC2PC(){

    BEAM=data

    DESTDIR=$DATADIR;
    Mode=data;
    MakeLinks;

    DESTDIR=$ONLINEDIR/log;
    Mode=log;
    MakeLinks;

    DESTDIR=$ONLINEDIR/hbook;
    Mode=hbook;
    MakeLinks;

}


#############################################################################
#                               Custom Dir                                  #
#############################################################################

CustomDir(){


    DESTDIR=$DATADIR;
    Mode=data;
    MakeLinks;

    DESTDIR=$ONLINEDIR/log;
    Mode=log;
    MakeLinks;

    DESTDIR=$ONLINEDIR/hbook;
    Mode=hbook;
    MakeLinks;

}



#############################################################################
#                                    Simple                                 #
#############################################################################
Simple(){

cd $DATADIR
echo "$DATADIR: making data  links ..."
ln.sh
ls ????.???.data | sed -e 's/.data//' > $TMPOUTDIR/runlist.dat;
sort $TMPOUTDIR/runlist.dat > $RAW_DATALIST;
rm -f $TMPOUTDIR/runlist.dat;
cd ..

cd $ONLINEDIR/hbook
echo  "$ONLINEDIR/hbook: making hbook links ..."
ln.sh
cd ..

cd $ONLINEDIR/log
echo "$ONLINEDIR/log: making  log  links ..."
ln.sh


}



#############################################################################
#                                    Main                                   #
#############################################################################
 
while test $# -ne 0; do
  case "$1" in
  --pc2pc-stand-alone) ExeStandAlonePC2PC=1 ;;
  --double-pc) ExeDoublePC=1 ;; 
  --pb2pb-2005) ExeSimple=1 ;;
  --dir) shift ; BEAM=$1 ; ExeCustomDir=1 ;;
  -x) shift ; ShowExample ;;
  -h | --help) help ;;
  *)  echo "Error: Invarid Option $1"
      help;;
  esac
  shift
done


if [ $ExeStandAlonePC2PC -eq 1 ] ; then
    StandAlonePC2PC;
fi
if [ $ExeDoublePC -eq 1 ] ; then
   DoublePC;
fi;
if [ $ExeSimple -eq 1 ] ; then
    Simple;
fi
if [ $ExeCustomDir -eq 1 ] ; then
    CustomDir;
fi

