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
	$nb_point = 1500;	//nombre de point du graphe
    $now  = time();
    $past = strtotime("-$nb_days day", $now);

    $db = new SQLite3($sqlite);
	$results = $db->querySingle("SELECT count(timestamp) FROM temp WHERE timestamp > $past ORDER BY timestamp ASC;");
	$numRows = $results;
	$modulo_nb_point = $nb_point / $numRows;
	$nb_point_effectif = 1;
	$num_point = 0;
	$results = $db->query("SELECT (timestamp -3600) as timestamp, temp, consigne FROM temp WHERE timestamp > $past ORDER BY timestamp ASC;");
	$data = array();
	while($row = $results->fetchArray(SQLITE3_ASSOC)){
		//modulo pour faire "sauter" des points
		$num_point += $modulo_nb_point;
		if ($num_point < $nb_point_effectif){
			continue;
		}
		$nb_point_effectif++;
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

  //
  //	récupere un champ de la table config
  //
  function getconfig($champ) {
	global $sqlite;
    $db = new SQLite3($sqlite); 
	$db->exec('CREATE TABLE IF NOT EXISTS config (config_key INTEGER, mode TEXT, T_manuelle FLOAT, N_planning integer, calibration FLOAT, hysteresis FLOAT, UNIQUE(config_key));'); // cree la table config si elle n'existe pas
	$results = $db->query("select " . $champ . " from config;");
    $data = array();
    $row = $results->fetchArray(SQLITE3_ASSOC);
    $data[] = $row[$champ];	
    return implode(', ', $data); 
  }
  
  //
  //	enregistre un champ de la table config
  //
  function setconfig($champ, $value) {
	global $sqlite;
    $db = new SQLite3($sqlite); 
	$db->exec('INSERT OR IGNORE INTO config (config_key, ' . $champ . ') Values (1, "' . $value . '"); UPDATE config set ' . $champ . ' = "' . $value . '" WHERE config_key = 1;'); 
	return 1;
	}
  
  //
  //	page de configuration du planning
  //
  function configPlanning ($temp, $consigne, $timestamp) {
    global $sqlite;
    $db = new SQLite3($sqlite);
    $db->exec('CREATE TABLE IF NOT EXISTS planning (N_planning INTEGER, nom TEXT, Jour INTEGER, H0 DATE, T0 FLOAT, H1 DATE, T1 FLOAT, H2 DATE, T2 FLOAT, H3 DATE, T3 FLOAT, H4 DATE, T4 FLOAT, H5 DATE, T5 FLOAT, H6 DATE, T6 FLOAT, H7 DATE, T7 FLOAT, H8 DATE, T8 FLOAT, H9 DATE, T9 FLOAT, H10 DATE, T10 FLOAT, UNIQUE(N_planning, Jour) );'); // cree la table planning si elle n'existe pas
  }
  
  //
  //	renvoie le planning à l'ESP en fonction du planning selectionné
  //
  function returnConfig ($temp, $consigne, $timestamp) {
    global $sqlite;
    $db = new SQLite3($sqlite);

  }
?>
