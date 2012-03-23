<html>
<head>
   <title>test</title>
   <link REL="STYLESHEET" TYPE="text/css" HREF="main.css.php">
   <base href="http://yellowpc.rhic.bnl.gov/rundb/oo/?" />
</head>

<body bgcolor="#ffffff">

<script  type="text/javascript">

var aEls = document.getElementsByTagName('a');

//for (var i = 0, aEl; aEl = aEls[i]; i++) {
   //aEl.href = aEl.href.replace('example.com','example2.com');
   //aEl.href = "http://ggg/q=" + aEl.href;
//}

</script>

<a href="hhh.tml">hhh</a>


<?
//print("ttt");

include("config.php");
include("utils.php"); 

//print_r($normJetCarbonByTarget); 

$normSpecs = array(
   "runPeriod" => 11,
	"energy"    => 250,
	"polId"     => "Y1D",
	"tgtOrient" => "H",
	"tgtId"     => 3
);

$normNew = getJetCarbonNormalization($normSpecs);

print("norm = $normNew\n");
?>

</body>
</html>
