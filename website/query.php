
<?php

$paramDb = test_GETSetOrDefault("db", "weather");
$paramUser = test_GETSetOrDefault("u", "weather_user");
$paramPassword = test_GETSetOrDefault("p", "weather_password");
$paramQuery = test_GETSetOrDefault("query", test_GETSetOrDefault("q", "LIST+SERIES"));	//Chain them together to test "query" or "q"

//This is cluster admin permission, only use if you need cluster admin functionality
//$getURL = "http://weatherspot.us:8086/db/" . $paramDb . "/series?u=root&p=root&q=" . $paramQuery;		
$getURL = "http://weatherspot.us:8086/db/" . $paramDb . "/series?u=" . $paramUser . "&p=" . $paramPassword . "&q=" . $paramQuery; 
doCurl($getURL);

function test_GETSetOrDefault($test, $default) {
	// Test if a parameter is set in the URL, otherwise return default value
	if (isset($_GET[$test])) {
		return rawurlencode($_GET[$test]);
	} else {
		return $default; 
	}
}
function doCurl($url) {
	$curl = curl_init();
	curl_setopt($curl, CURLOPT_URL, $url); 
	$result = curl_exec($curl);
	curl_close($curl);
}
?>
