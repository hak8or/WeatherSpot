// Location.js depends upon /js/graphFactory.js

(function(WeatherSpot_Map, $, undefined) {
	function initialize() {

		//Using the name of the series as the name of a spot, add a new location using addLocation(...) and it will auto-populate on the map 
		//If you use a series that does not exist, it will not appear on the map
		var locations = {}; 
		function addLocation(_name, _lat, _long) {
			_lat = _lat || 40.700815;
			_long = _long || -73.892049;
			locations[_name] = new google.maps.LatLng(_lat, _long);
		}
		///////////
		addLocation("Staten_Island", 40.586510, -74.100813);
		addLocation("Manhattan", 40.767875, -73.964059);
		addLocation("Queens", 40.751447, -73.900095);
		///////////

		var mapCenter = new google.maps.LatLng(40.690366, -74.045903);
		var mapOptions = {
			zoom: 10,
			center: mapCenter 
		};
		var map = new google.maps.Map(document.getElementById('map-canvas'), mapOptions);

		var infowindow = new google.maps.InfoWindow();
		google.maps.event.addListener(infowindow, 'closeclick', function() {
			//Unhighlight the marker_list items
//			$("#marker_list li").css({"background-color": "#efefef"});
			$("#marker_list li").removeClass("list_highlight");
		});

		function makeMarkerWithLabelCallback(db, data) {
			console.log("Data: " + data);
			var seriesName = data[0].name;
			var measurement = data[0].points[0][2]; 
			makeMarkerWithLabel(seriesName, measurement.toString(), locations[seriesName]);
		}
		var image = 'pic/cloud.png';
		function createMarkerButton(marker) {
			//Creates a sidebar button
			var ul = document.getElementById("marker_list");
			var li = document.createElement("li");
			var title = marker.getTitle();
			li.innerHTML = title;
			ul.appendChild(li);
			
			//Trigger a click event to marker when the button is clicked.
			google.maps.event.addDomListener(li, "click", function(){
//				$("#marker_list li").css({"background-color": "#efefef"});		//Unhighlight all list items
				$("li.list_highlight").removeClass("list_highlight");			//Unhighlight all list items
//				$(this).css({"background-color": "yellow"});				//Highlight clicked item
				$(this).addClass("list_highlight");					//Highlight clicked item
				google.maps.event.trigger(marker, "click");				//Forward click event to the marker
			});
		}	
		function makeMarkerWithLabel(_title, _content, _where) {
			console.log("Title: " + _title + "; Content: " + _content + "; Where: " + _where);
				 
			var cloudmarker = new google.maps.Marker({
				position: _where,
				map: map,
				icon: image,
				title: _title, 
				animation: google.maps.Animation.DROP
			});
			createMarkerButton(cloudmarker);
                        var label = new Label({
                            map: map,
                            label: _content
                        });
                        label.bindTo('position', cloudmarker, 'position');
                        label.bindTo('text', cloudmarker, 'position');

			var graphFactory = GraphFactory("div#graph-container");
			console.log(graphFactory);
			google.maps.event.addListener(cloudmarker, 'click', function() {
				infowindow.setContent("<strong>" + _title + ": " + _content + "</title>");
				infowindow.open(map,cloudmarker);
				graphFactory.removeGraphs();
				graphFactory.graphFourMeasurements(_title);
				var offset = $('#graph-container').offset();
				offset.left -= 20;
				offset.top -= 40;
				setTimeout(function() {
					$('html, body').animate({
					    scrollTop: offset.top,
					    scrollLeft: offset.left
					});
				}, 750);
			});
		}
		function doQuery(db, query, callback) {
			console.log(query);
			var queryURL = "http://weatherspot.us/db/query.php?db=" + db + "&query=" + encodeURIComponent(query);
			$.ajax({
				type: 'GET',
				url: queryURL,
				success: function(data) {
					callback(db, data);
				},
				error: function(jqXHR, textStatus, errorThrown) {
				},
				dataType: "json"
			});
		}

		for (var loc in locations) {
			var latestTemperature = "SELECT temperature FROM " + loc + " LIMIT 1";
			doQuery("weather", latestTemperature, makeMarkerWithLabelCallback); 
		}
		console.log("Last Loc: " + loc);

	}
	google.maps.event.addDomListener(window, 'load', initialize);

	return {
		initialize: initialize
	}
}( window.WeatherSpot_Map = window.WeatherSpot_Map || {}, jQuery));
