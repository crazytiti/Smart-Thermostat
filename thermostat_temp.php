<?php
require_once('/var/www/html/thermostat/thermostat_func.php');

putTemp($_GET['temp'], $_GET['consigne'], $_GET['timestamp']);

echo'temp:';
echo getActualTemp ();
echo'°c<br>';

?>
