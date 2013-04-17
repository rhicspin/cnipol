
<?php

include_once("utils.php");


class MeasResult
{
   public $fRunPeriod;


   function MeasResult()
	{
	}


	function GetBunchUpDownCounts(&$beamBunches)
	{
      $udCounts = array('-' => 0, '0' => 0, '+' => 0);

      foreach ($beamBunches as $bunchId => $bunch)
		{
		   if ( !empty($bunch['fIsFilled']) && intval($bunch['fBunchSpin']) == +1 )
			   $udCounts['+']++;

		   if ( !empty($bunch['fIsFilled']) && intval($bunch['fBunchSpin']) == -1 )
			   $udCounts['-']++;

		   if ( empty($bunch['fIsFilled']) )
			   $udCounts['0']++;
		}

		return $udCounts;
	}
}

?>
