<?php

  $sqlite = '/home/pi/thermostat/thermostat.sqlite';

  //
  //  enregistre une mesure de température
  //
  function putTemp ($temp, $consigne, $timestamp) {
    global $sqlite;
    $db = new SQLite3($sqlite);
    $db->exec('CREATE TABLE IF NOT EXISTS temp (timestamp INTEGER, temp FLOAT, consigne FLOAT);'); // cree la table temp si elle n'existe pas
    $data = array();
    $data['timestamp'] = $timestamp;
    $data['temp']  = $temp;
	$data['consigne']  = $consigne;	
    if($db->busyTimeout(5000)){ // stock les donnees
      $db->exec("INSERT INTO temp (timestamp, temp, consigne) VALUES ('".$data['timestamp']."', '".$data['temp']."', '".$data['consigne']."');");
	  echo "OK";
	}
    return 1;
  }

  //
  //	genere le tableau d'historique de température
  //
  function getTempGraph ($nb_days) {
    global $sqlite;
    $now  = time();
    $past = strtotime("-$nb_days day", $now);

    $db = new SQLite3($sqlite);
    $results = $db->query("SELECT (timestamp -3600) as timestamp, temp, consigne FROM temp WHERE timestamp > $past ORDER BY timestamp ASC;");

    $data = array();

    while($row = $results->fetchArray(SQLITE3_ASSOC)){
      $year   = date("Y", $row['timestamp']);
      $month = date("n", $row['timestamp'])-1;
      $day    = date("j", $row['timestamp']);
      $hour   = date("G", $row['timestamp']);
      $minute = date("i", $row['timestamp']);
      $second = date("s", $row['timestamp']);
      $temp_indicator ='color: #e0440e';
	  $consigne_indicator = 'color: #375D81';
	  
      $data[] = "[{v:new Date($year, $month, $day, $hour, $minute, $second), f:'".date("j", $row['timestamp'])." ".date("M", $row['timestamp'])." ".date("H\hi", $row['timestamp'])."'}, 
                  {v:".$row['temp'].", f:'".$row['temp']." °c'}, '".$temp_indicator."', {v:".$row['consigne'].", f:'".$row['consigne']." °c'}]";
	//  $data[] = "[{v:new Date($year, $month, $day, $hour, $minute, $second), f:'".date("j", $row['timestamp'])." ".date("M", $row['timestamp'])." ".date("H\hi", $row['timestamp'])."'}, 
    //              {v:".$row['temp'].", f:'".$row['temp']." °c'}, '".$consigne_indicator."', {v:".$row['consigne'].", f:'".$row['consigne']." °c'}]";
    }

    return implode(', ', $data);
  }
  
  //
  // recupere la dernierre température pour faire du "temps réel"
  //
  function getActualTemp() {
	global $sqlite;
    $db = new SQLite3($sqlite);
    $results = $db->query("select max(timestamp) as timestamp,temp from temp;");
    $data = array();
    $row = $results->fetchArray(SQLITE3_ASSOC);
    $data[] = $row['temp'];	
    return implode(', ', $data); 
  }
?>
