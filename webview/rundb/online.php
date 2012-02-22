<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN">

<!--
******************************************************************************
*
******************************************************************************
-->

<html>
<head>
  <title>Run Database: RHIC p-Carbon polarimeters</title>
  <link REL="STYLESHEET" TYPE="text/css" HREF="../main.css">
</head>

<body bgcolor="#ffffff">

<?php

if (isset($_GET['runid']))
{
   $runId = $_GET['runid'];
}

?>

<table class="simple" cellspacing=10 align=center>
<tr>
<td>
<img src="../log/<?=$runId?>.a.png">
<td>
<img src="../log/<?=$runId?>.b.png">
</table>

</body>
</html>
