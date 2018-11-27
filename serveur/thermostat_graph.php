<center>
	<img src="./ban.png" img style="border: 0px solid ; height:150px; ">
</center>
<br>
<body bgcolor='#ADD8E6'>
<?php require_once('/var/www/html/teleinfo_func.php'); ?>

<script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>

<div id="puissance">
  <div id="chart_div"></div>
  <div id="filter_div"></div>
</div>
<div id="daily_div"></div>
<div id="monthly_div"></div>

<script type="text/javascript">
  google.charts.load('current', {packages: ['corechart', 'controls'], 'language': 'fr'});
  google.charts.setOnLoadCallback(drawDashboard);

  function drawDashboard() {

    var data = new google.visualization.DataTable();
        data.addColumn('date', 'Date');
        data.addColumn('number', 'V.A');
        data.addColumn({type:'string', role:'style'});
        data.addColumn('number', 'W');
        data.addRows([<?php echo getInstantConsumption (4); ?>]);

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
                             title: 'Consommation Instantanée : <?php echo getActualConsumption (); ?>',
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
  
  google.charts.setOnLoadCallback(drawChart);
  
  function drawChart() {
    var data = new google.visualization.DataTable();
    data.addColumn('date', 'Date');
    data.addColumn('number', 'Heures pleines');
    data.addColumn('number', 'Heures creuses');
    data.addRows([<?php echo getDailyData (365); ?>]);
    var dailyChartOptions = {
                   title: 'Consommation journalière',
                   height : 200,
                   backgroundColor: '#FFF',
                   colors : ['#375D81', '#ABC8E2'],
                   focusTarget : 'category',
                   isStacked: true,
                   legend : {position: 'bottom',  alignment: 'center', textStyle: {color: '#333', fontSize: 16}},
                   vAxis : {textStyle : {color : '#555', fontSize : '16'}, gridlines : {color: '#CCC', count: 'auto'}, baselineColor : '#AAA', minValue : 0},
                   hAxis : {textStyle : {color : '#555'}, gridlines : {color: '#DDD'}}
              };

function floorDate(date) {
      var month = new Date(date.getFullYear(), date.getMonth());
      return month;
    }

    var monthlyData = google.visualization.data.group(data, [{
      column: 0,
      modifier: floorDate,
      type: 'date'
    }], [{
        column: 1,
        label: 'Heures pleines',
        aggregation: google.visualization.data.sum,
        type: 'number'
    }, {
        column: 2,
        label: 'Heures creuses',
        aggregation: google.visualization.data.sum,
        type: 'number'
    }]);

    // Format dates
    var formatter = new google.visualization.DateFormat({pattern: "MMM ' 'yyyy"});
    formatter.format(monthlyData, 0);

    var monthlyChartOptions = {
                   title: 'Consommation mensuelle',
                   height : 200,
                   backgroundColor: '#FFF',
                   colors : ['#375D81', '#ABC8E2'],
                   focusTarget : 'category',
                   isStacked: true,
                   legend : {position: 'bottom',  alignment: 'center', textStyle: {color: '#333', fontSize: 16}},
                   vAxis : {textStyle : {color : '#555', fontSize : '16'}, gridlines : {color: '#CCC', count: 'auto'}, baselineColor : '#AAA', minValue : 0},
                   hAxis : {textStyle : {color : '#555'}, gridlines : {color: '#DDD'}, format: 'MMM yyyy'}
              };

    var dailyChart = new google.visualization.ColumnChart(document.getElementById("daily_div"));
    dailyChart.draw(data, dailyChartOptions);

    var monthlyChart = new google.visualization.ColumnChart(document.getElementById("monthly_div"));
    monthlyChart.draw(monthlyData, monthlyChartOptions);
  }

</script>
