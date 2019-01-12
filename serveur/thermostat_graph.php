<center>
	<img src="./ban.png" img style="border: 0px solid ; height:150px; ">
</center>
<br>
<body bgcolor='#ADD8E6'>
<?php require_once('/var/www/html/thermostat/thermostat_func.php'); ?>

<script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>

<div id="puissance">
  <div id="chart_div"></div>
  <div id="filter_div"></div>
</div>

<script type="text/javascript">
  google.charts.load('current', {packages: ['corechart', 'controls'], 'language': 'fr'});
  google.charts.setOnLoadCallback(drawDashboard);

  function drawDashboard() {

    var data = new google.visualization.DataTable();
        data.addColumn('date', 'Date');
        data.addColumn('number', 'Température');
        data.addColumn({type:'string', role:'style'});
        data.addColumn('number', 'Consigne');
        data.addRows([<?php echo getTempGraph (4); ?>]);

    var dashboard = new google.visualization.Dashboard(
            document.getElementById('puissance'));

    var rangeSlider = new google.visualization.ControlWrapper({
          'controlType': 'ChartRangeFilter',
          'containerId': 'filter_div',
          'options': {
             filterColumnLabel : 'Date',
             ui : {chartType: 'LineChart', chartOptions: {
                             height : 80,
                             backgroundColor: '#FFF',
                             colors : ['#375D81', '#ABC8E2'],
                             curveType : 'function',
                             focusTarget : 'category',
                             lineWidth : '1',
                             'legend': {'position': 'none'},
                             'hAxis': {'textPosition': 'in'},
                             'vAxis': {
                               'textPosition': 'none',
                               'gridlines': {'color': 'none'}
                             }
                 }}
          }
        });
        
    var lineChart = new google.visualization.ChartWrapper({
          'chartType': 'LineChart',
          'containerId': 'chart_div',
          'options': {
                             title: 'Température actuelle : <?php echo getActualTemp (); ?>°c. Historique :',
                             height : 400,
                             backgroundColor: '#FFF',
                             colors : ['#375D81', '#ABC8E2'],
                             curveType : 'function',
                             focusTarget : 'category',
                             lineWidth : '1',
                             legend : {position: 'bottom',  alignment: 'center', textStyle: {color: '#333', fontSize: 16}},
                             vAxis : {textStyle : {color : '#555', fontSize : '16'}, gridlines : {color: '#CCC', count: 'auto'}, baselineColor : '#AAA', minValue : 0},
                             hAxis : {textStyle : {color : '#555'}, gridlines : {color: '#DDD'}}
          }
        });

    dashboard.bind(rangeSlider, lineChart);
    dashboard.draw(data);

  }

</script>
