<?php
 
include_once("config.php"); 
include_once("utils.php"); 
include_once("RunSelector.php");

$host = 'localhost'; // MYSQL database host adress
$db   = 'cnipol'; // MYSQL database name
$user = 'cnipol'; // Mysql Datbase user
$pass = '(n!P0l'; // Mysql Datbase password

// Connect to the database
$link = mysql_connect($host, $user, $pass);

//print_r($_GET);

$runSelector = new RunSelector();

mysql_select_db($db);

$table = "run_info"; // this is the tablename that you want to export to csv from mysql.

exportMysqlToCsv($table, $runSelector->sqlWhere);
 
?>
