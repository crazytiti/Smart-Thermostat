<?php
require_once('/var/www/html/thermostat/thermostat_func.php');

//echo(getconfig("N_planning"));
echo(getPlanningDayImplode($_GET["jour"], getconfig("N_planning")));

?>
