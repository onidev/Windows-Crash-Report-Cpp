<?php

$version = $_GET['version'];
$error   = $_GET['error'];
$count   = 1;

// arguments are required
if($version == NULL || $error == NULL)
{
    die();
}

// protect from sql injections - accepts max unsigned int value (4294967295)
if( !ctype_digit($version) || !ctype_digit($error) )
{
    die();
}

// connect to database
$mysqli = new mysqli("host", "username", "password", "dbname");
$table  = "my_game_crash_report"

if($mysqli->connect_error)
	die ($mysqli->connect_error);

// Check if crash adress exists
if($ans = $mysqli->query("select count from ".$table." where error=".$error.";"))
{
    $datarow = $ans->fetch_array();
    $count = $datarow[0];
    
    if($count == NULL)
    {
        $count = 1;
    }
    else
    {
        $count++;
        $mysqli->query("delete from ".$table." where error=".$error.";");
    }
}

// Insert crash report in database
$mysqli->query("insert into ".$table." VALUES (".$version.", '".$error."', ".$count.");");
$mysqli->close();

?>
