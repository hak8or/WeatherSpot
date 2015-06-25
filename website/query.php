
<?php

if (isset($_GET["db"])) {
        $paramDb = rawurlencode($_GET["db"]);
}
else {
        $paramDb = "weather";
}
if (isset($_GET["u"])) {
	$paramUser = rawurlencode($_GET["u"]);
} else {
        $paramUser = "weather_user";
}
if (isset($_GET["p"])) {
	$paramPassword = rawurlencode($_GET["p"]);
} else {
        $paramPassword = "weather_password";
}
if (isset($_GET["query"])) {
	$paramQuery = rawurlencode($_GET["query"]);
} else if (isset($_GET["q"])) {
	$paramQuery = rawurlencode($_GET["q"]);
} else {
        $paramQuery = rawurlencode("LIST SERIES");
}

//This is cluster admin permission, only use if you need cluster admin functionality
//$getURL = "http://weatherspot.us:8086/db/" . $paramDb . "/series?u=root&p=root&q=" . $paramQuery;		
$getURL = "http://weatherspot.us:8086/db/" . $paramDb . "/series?u=" . $paramUser . "&p=" . $paramPassword . "&q=" . $paramQuery; 

$curl = curl_init();
curl_setopt($curl, CURLOPT_URL, $getURL); 
$result = curl_exec($curl);
curl_close($curl);
?>
