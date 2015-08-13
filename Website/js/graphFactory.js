var GraphFactory = function(_appendTo) {
	// Class GraphFactory
	// Purpose: graphing queries
	// Includes:
	//	<script src="//code.jquery.com/jquery-1.11.2.min.js"></script>
	//	<script src="//code.jquery.com/jquery-migrate-1.2.1.min.js"></script>
	//	<script src="//code.jquery.com/ui/1.11.4/jquery-ui.js"></script>
	//	<script src="/js/queryFactory.js"></script>
	//	<script src="/js/jquery.timepicker.min.js"></script>
	//	<link rel="stylesheet" type="text/css" href="/css/jquery.timepicker.css">
	//	<link rel="stylesheet" type="text/css" href="/css/graphs.css">
	//	<link rel="stylesheet" type="text/css" href="https://code.jquery.com/ui/1.11.4/themes/dark-hive/jquery-ui.css">

	/////////////
	// Private //
	/////////////
	var gAppendToElem = _appendTo; 
	var cachedPoints = [];

	////////////
	// Public //
	////////////
	// Graph the four Weatherspot measurements
	this.graphFourMeasurements = function(series) {
		var q = function(measurements) { 
			var r = [];
			for (var i=0,len=measurements.length; i<len; ++i ) {
				r.push("SELECT mean(" + measurements[i] + ") FROM " + series + " GROUP BY time(10m) WHERE time > now() - 24h");
			}
			return r;
		}(['temperature','humidity','pressure','lighting']);

		cachedPoints = [];	//Clear cached points
		graphQuery(q[0], "red");
		graphQuery(q[1], "blue");
		graphQuery(q[2], "green");
		graphQuery(q[3], "orange");
	}
	this.removeGraphs = function() {
		$(gAppendToElem).children("svg").remove();
	}
	this.graphQuery = function(query, color) {
		color = color || 'blue';
		var queryURL = "/db/query.php?db=weather&query=" + encodeURIComponent(query); 

		// Set the dimensions of the canvas / graph
		var margin = {top: 30, right: 20, bottom: 50, left: 60},
		    width = $(gAppendToElem).width() * .9 - margin.left - margin.right,
		    height = 270 - margin.top - margin.bottom;

		// Parse the date / time
		var parseDate = d3.time.format("%d-%b-%H-%M-%S").parse;

		// Set the ranges
		var x = d3.time.scale().range([0, width]);
		var y = d3.scale.linear().range([height, 0]);

		// Define the axes
		var xAxis = d3.svg.axis().scale(x)
		    .orient("bottom").ticks(width / 60 );

		var yAxis = d3.svg.axis().scale(y)
		    .orient("left").ticks(5);

		// Define the line
		var valueline = d3.svg.line()
		    .x(function(d) { return x(d.time); })
		    .y(function(d) { return y(d.point); });
		    
		// Adds the svg canvas
		var svg = d3.select(gAppendToElem)
		    .append("svg")
			.attr("width", width + margin.left + margin.right)
			.attr("height", height + margin.top + margin.bottom)
//			.attr("viewBox", "0 0 " + width + " " + height)		// I dont know how exactly to scale here
		    .append("g")
			.attr("transform", 
			      "translate(" + margin.left + "," + margin.top + ")");

		var strIndex = query.indexOf(" FROM");
		var field = query.substr("SELECT ".length, strIndex - "SELECT ".length);
		field = field.charAt(0).toUpperCase() + field.slice(1);

		// Get the data
		d3.json(queryURL, function(error, data) {
		    if (data === undefined) {
			console.log("Bad Query: " + queryURL);
			$("#failText").html("Bad Query");
			$("svg:nth-last-of-type(1)").remove();
			return;
		    }
		    if (typeof data[0] === 'undefined') {
			console.log("No Datas! " + queryURL);
			var $failText = $('#failText');
			$failText.html("No Data Returned");
			$failText.css({"background-color": "red", "opacity": 1});
			$failText.animate({"background-color": "", "opacity": 0}, 2500);
			$("svg:nth-last-of-type(1)").remove();
			return;
		    }
		    $("#failText").html("");
		    points = data[0]["points"];
		    //console.log(data);
		    points.forEach(function(d) {
			d.time = d[0];
			if (d.length < 3) {
				d.point = d[1];
			} else {
				d.point = d[2];
			}
		    });

		    // Scale the range of the data
		    x.domain(d3.extent(points, function(d) { return d.time; }));
		    y.domain([d3.min(points, function(d) { return d.point; }) * .99, d3.max(points, function(d) { return d.point; }) * 1.01]);

		    // Add the valueline path.
		    svg.append("path")
			.attr("class", "line")
			.attr("d", valueline(points))
			.attr("style", "stroke:"+color);

		    // Add the X Axis
		    svg.append("g")
			.attr("class", "x axis")
			.attr("transform", "translate(0," + height + ")")
			.call(xAxis);

		    // Add the Y Axis
		    svg.append("g")
			.attr("class", "y axis")
			.call(yAxis);

			cachedPoints.push({data: points, color: color, field: field});
		});
//		var strIndex = query.indexOf(" FROM");
//		var field = query.substr("SELECT ".length, strIndex - "SELECT ".length);
//		field = field.charAt(0).toUpperCase() + field.slice(1);
		// Label From: http://www.d3noob.org/2013/01/adding-title-to-your-d3js-graph.html
		svg.append("text")
			.attr("x", (width / 2))             
			.attr("y", 0 - (margin.top / 2))
			.attr("text-anchor", "middle")  
			.style("font-size", "16px") 
			.style("text-decoration", "underline")  
			.text(getTitleString(field) + " vs Time");
	}
	this.graphData = function(cachedPoint) {
		var color = cachedPoint.color || 'blue';
		var points = cachedPoint.data;
		var field = cachedPoint.field;

		// Set the dimensions of the canvas / graph
		var margin = {top: 30, right: 20, bottom: 50, left: 60},
		    width = $(gAppendToElem).width() * .9 - margin.left - margin.right,
		    height = 270 - margin.top - margin.bottom;

		// Parse the date / time
		var parseDate = d3.time.format("%d-%b-%H-%M-%S").parse;

		// Set the ranges
		var x = d3.time.scale().range([0, width]);
		var y = d3.scale.linear().range([height, 0]);

		// Define the axes
		var xAxis = d3.svg.axis().scale(x)
		    .orient("bottom").ticks(width / 60 );

		var yAxis = d3.svg.axis().scale(y)
		    .orient("left").ticks(5);

		// Define the line
		var valueline = d3.svg.line()
		    .x(function(d) { return x(d.time); })
		    .y(function(d) { return y(d.point); });
		    
		// Adds the svg canvas
		var svg = d3.select(gAppendToElem)
		    .append("svg")
			.attr("width", width + margin.left + margin.right)
			.attr("height", height + margin.top + margin.bottom)
//			.attr("viewBox", "0 0 " + width + " " + height)		// I dont know how exactly to scale here
		    .append("g")
			.attr("transform", 
			      "translate(" + margin.left + "," + margin.top + ")");

		    points.forEach(function(d) {
			d.time = d[0];
			if (d.length < 3) {
				d.point = d[1];
			} else {
				d.point = d[2];
			}
		    });

		    // Scale the range of the data
		    x.domain(d3.extent(points, function(d) { return d.time; }));
		    y.domain([d3.min(points, function(d) { return d.point; }) * .99, d3.max(points, function(d) { return d.point; }) * 1.01]);

		    // Add the valueline path.
		    svg.append("path")
			.attr("class", "line")
			.attr("d", valueline(points))
			.attr("style", "stroke:"+color);

		    // Add the X Axis
		    svg.append("g")
			.attr("class", "x axis")
			.attr("transform", "translate(0," + height + ")")
			.call(xAxis);

		    // Add the Y Axis
		    svg.append("g")
			.attr("class", "y axis")
			.call(yAxis);

		field = field.charAt(0).toUpperCase() + field.slice(1);
		// Label From: http://www.d3noob.org/2013/01/adding-title-to-your-d3js-graph.html
		svg.append("text")
			.attr("x", (width / 2))             
			.attr("y", 0 - (margin.top / 2))
			.attr("text-anchor", "middle")  
			.style("font-size", "16px") 
			.style("text-decoration", "underline")  
			.text(getTitleString(field) + " vs Time");
	}

	var getTitleString = function(field) {
		console.log(field);
		var str = field.replace(/mean\((.*)\)/i, function(m, $1) { return $1; });
		str = (function(s) { return s.charAt(0).toUpperCase() + s.slice(1); })(str);
		console.log(str);
		return str;
	}
	var handleResize = function() {
		var offset = $(window).scrollTop(); 
		removeGraphs();
		for (var index=0, len=cachedPoints.length; index<len; ++index) {
			console.log(cachedPoints[index]);
			graphData(cachedPoints[index]);
		}
		$('html, body').scrollTop(offset);
	}
	window.onresize = handleResize;
	return this;
}
