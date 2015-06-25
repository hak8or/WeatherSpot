
<?php

if (isset($_GET["db"])) {
        $paramDb = rawurlencode($_GET["db"]);
}
else {
        $paramDb = "weather";
}
if (isset($_GET["query"])) {
	$paramQuery = rawurlencode($_GET["query"]);
}
else {
        $paramQuery = rawurlencode("LIST SERIES");
}

//This is cluster admin permission, only use if you need cluster admin functionality
//$getURL = "http://weatherspot.us:8086/db/" . $paramDb . "/series?u=root&p=root&q=" . $paramQuery;		
$getURL = "http://weatherspot.us:8086/db/" . $paramDb . "/series?u=weather_user&p=weather_password&q=" . $paramQuery; 

$curl = curl_init();
curl_setopt($curl, CURLOPT_URL, $getURL); 
$result = curl_exec($curl);
curl_close($curl);
?>
