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
	}
    return 1;
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
