
<?php

if (isset($_GET["db"])) {
        $paramDb = htmlspecialchars($_GET["db"]);
}
else {
        $paramDb = "weather";
}
if (isset($_GET["query"])) {
//	$paramQuery = $_GET["query"];
	$paramQuery = urlencode($_GET["query"]);
}
else {
        $paramQuery = "SELECT%20*%20from%20Downtown";
}

$getURL = "http://weatherspot.us:8086/db/" . $paramDb . "/series?u=root&p=root&q=" . $paramQuery; 

$curl = curl_init();
curl_setopt($curl, CURLOPT_URL, $getURL); 
$result = curl_exec($curl);
curl_close($curl);

echo $result[0];
?>
