
<?php

$method = $_SERVER['REQUEST_METHOD'];
switch ($method) {
	case "GET":
		header("Cache-Control: max-age:300"); //HTTP 1.1

		$paramDb = test_GETSetOrDefault("db", "weather");
		$paramUser = test_GETSetOrDefault("u", "weather_user");
		$paramPassword = test_GETSetOrDefault("p", "weather_password");
		$paramQuery = test_GETSetOrDefault("query", test_GETSetOrDefault("q", "LIST+SERIES"));	//Chain them together to test "query" or "q"

		//This is cluster admin permission, only use if you need cluster admin functionality
		//$getURL = "http://weatherspot.us:8086/db/" . $paramDb . "/series?u=root&p=root&q=" . $paramQuery;		
		$getURL = "http://weatherspot.us:8086/db/" . $paramDb . "/series?u=" . $paramUser . "&p=" . $paramPassword . "&q=" . $paramQuery; 
		doGETCurl($getURL);
	break;
	case "POST":
		// Example for CURLing to write data:
		// $> curl -X POST 'http://weatherspot.us/db/query.php?series=Queens_County&temperature=85&humidity=75&pressure=99&lighting=25'
	
		$paramSeries = test_GETSetOrDefault("series", "");
		$paramTemperature = test_GETSetOrDefault("temperature", "");
		$paramHumidity = test_GETSetOrDefault("humidity", "");
		$paramPressure = test_GETSetOrDefault("pressure", "");
		$paramLighting = test_GETSetOrDefault("lighting", "");
		$paramData = buildJSON($paramSeries, $paramTemperature, $paramHumidity, $paramPressure, $paramLighting);
		if ($paramData == "") {
			echo "Bad Data";
			break;
		}

		$paramDb = test_GETSetOrDefault("db", "weather");
		$paramUser = test_GETSetOrDefault("u", "weather_writer");
		$paramPassword = test_GETSetOrDefault("p", "weather_password");
		 
		$postURL = "http://weatherspot.us:8086/db/" . $paramDb . "/series?u=" . $paramUser . "&p=" . $paramPassword; 
		doPOSTCurl($postURL, $paramData);
	break;
}


function buildJSON($series, $temperature, $humidity, $pressure, $light) {
	if (!$series || !$temperature || !$humidity || !$pressure || !$light) {
		return "";
	}
	$json = '[{"name":"' . $series . '", "columns":["temperature","humidity","pressure","lighting"],"points":[[' . $temperature . ',' . $humidity . ',' . $pressure . ',' . $light . ']]}]';

	return $json;
}
function test_GETSetOrDefault($test, $default) {
	// Test if a parameter is set in the URL, otherwise return default value
	if (isset($_GET[$test])) {
		return rawurlencode($_GET[$test]);
	} else {
		return $default; 
	}
}
function test_POSTSetOrDefault($test, $default) {
	// Test if a parameter is set in the URL, otherwise return default value
	if (isset($_POST[$test])) {
		return $_POST[$test];
	} else {
		return $default; 
	}
}
function doGETCurl($url) {
	$s = curl_init();
	curl_setopt($s, CURLOPT_URL, $url); 
	$result = curl_exec($s);
	curl_close($s);
}
function doPOSTCurl($url, $data) {
	$s = curl_init();
	curl_setopt($s, CURLOPT_URL, $url); 
	curl_setopt($s,CURLOPT_POST,true); 
	curl_setopt($s,CURLOPT_POSTFIELDS,$data); 
	$result = curl_exec($s);
	curl_close($s);
}
?>
