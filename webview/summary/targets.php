<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<!--
******************************************************************************
*
******************************************************************************
-->

<html>
<head>
  <title>RHIC Polarimetry: Target Usage History</title>
  <link REL="STYLESHEET" TYPE="text/css" HREF="../main.css">
  <link REL="STYLESHEET" TYPE="text/css" HREF="../wiki.css">
  <link REL="STYLESHEET" TYPE="text/css" HREF="../wiki_monobook_main.css">
  <script language="javascript" src="../main.js"></script>
  <script language="javascript" src="../toc.js"></script>
</head>

<body bgcolor="#ffffff">

<?php
include("../head.html");

include_once("../rundb/DbRunInfo.php");

$dbRunInfo = new DbRunInfo();
?>

<!-- Main text starts here-->

<p><a name="content"/>


<h1>Target Usage History</h1>


<h2>Run 13</h2>

<h3 class=center>Feb 17, 2013 - ...</h3>

<?php
$targetUsage = $dbRunInfo->FetchTargetUsage("2013-02-17");
$dbRunInfo->PrintTargetUsage();
?>


<h2>Run 12</h2>


<h3 class=center>Jan 25, 2012 - Mar 13, 2012</h3>

<?php
$targetUsage = $dbRunInfo->FetchTargetUsage("2012-01-25 00:00:00", "2012-03-14 00:00:00");
$dbRunInfo->PrintTargetUsage();
?>


<h3 class=center>Mar 14, 2012 - Apr 10, 2012</h3>

<?php
$targetUsage = $dbRunInfo->FetchTargetUsage("2012-03-14 00:00:00", "2012-04-11 00:00:00");
$dbRunInfo->PrintTargetUsage();
?>


<h3 class=center>Apr 11, 2012 - Apr 19, 2012</h3>

<?php
$targetUsage = $dbRunInfo->FetchTargetUsage("2012-04-11 00:00:00", "2012-04-20 00:00:00");
$dbRunInfo->PrintTargetUsage();
?>


<h2>Run 11</h2>

<h3 class=center>Jan 4, 2011 - Apr 19, 2011</h3>

<?php
$targetUsage = $dbRunInfo->FetchTargetUsage("2011-01-04 00:00:00", "2011-04-20 00:00:00");
$dbRunInfo->PrintTargetUsage();
?>


<h2>Run 9</h2>

<h3 class=center>Jan 15, 2009 - Jul 6, 2009</h3>

<?php
$targetUsage = $dbRunInfo->FetchTargetUsage("2009-01-15 00:00:00", "2009-06-07 00:00:00");
$dbRunInfo->PrintTargetUsage();
?>


<?php
include("../bottom.html");
?>

</body>
</html>
