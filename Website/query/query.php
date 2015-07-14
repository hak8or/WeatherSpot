<?php

$method = $_SERVER['REQUEST_METHOD'];
$url = get_current_url(false);
dout($method . " " . $url); 
switch ($method) {
case "GET":
	//GET is for retrieving resources
	//Example: SELECT queries
	dout("<h4>Try \"/query/database/series/point/duration/\"</h4>");

	if (strpos($url, "?") > 0) {
		$urlPath = substr($url, strpos($url, "/query"), strpos($url, "?") - strpos($url, "/query/"));
		$urlQuery = substr($url, strpos($url, "?"));
		$displayFormat = substr($urlQuery, strpos($urlQuery, "format=") + 7);	//CHANGE THIS TO ACTUALLY PARSE FROM THE $urlQuery
	} else {
		$urlPath = substr($url, strpos($url, "/query/"));
		$urlQuery = "";
		$displayFormat = "json";
	}
	$numberOfSlashes = substr_count($urlPath, "/");
	dout("Slashes: " . $numberOfSlashes . "<br>");
	if ($urlQuery != "") {
		dout("Query Section: " . $urlQuery . "<br><br>");
		dout("Display Format: " . $displayFormat . "<br><br>");
	}

	$delimeters = tokenizeUrl($urlPath);

	// Based on the number of slashes in the path, the client is asking for a different response
	switch ($numberOfSlashes) {
		case 2:
			//This should print out a list of databases, but influxDB does not seem to support LIST DATABASES;
			if ($delimeters[1] == "") {
				dout("Available Databases: weather");
				break;
			}
			//Fall through if $delimeters[1] is not blank
		case 3:
			//Return with a LIST TABLES of given database
			if ($delimeters[2] == "") {
				$database = $delimeters[1]; 
				dout("Database: " . $database . "<br><br>");

				$q = "LIST SERIES";
				$curlURL = "http://weatherspot.us/db/query.php?db=" . $database . "&query=" . rawurlencode($q);
				$response = doCurl($curlURL);
				printFormatted($response, $displayFormat);
				break;
			}
		case 4:
			//Return with columns in a series
			if ($delimeters[3] == "") {
				$database = $delimeters[1]; 
				$series = $delimeters[2]; 
				dout("Database: " . $database . "<br><br>");
				dout("Series: " . $series . "<br><br>");

				if ($series == "all") {
					$series = "/.*/";
				}
				$q = "SELECT * from " . $series;
				$curlURL = "http://weatherspot.us/db/query.php?db=" . $database . "&query=" . rawurlencode($q);
				$response = doCurl($curlURL);
				printFormatted($response, $displayFormat);
				break;
			}
		case 5:
			//Return with all points in a series
			if ($delimeters[4] == "") {
				$database = $delimeters[1]; 
				$series = $delimeters[2]; 
				$point = $delimeters[3]; 

				dout("Database: " . $database . "<br><br>");
				dout("Series: " . $series . "<br><br>");
				dout("Points: " . $point . "<br><br>");
				if ($series == "all") {
					$series = "/.*/";
				}
				if ($point == "all") {
					$point = "*";
				}
				$q = "SELECT " . $point . " from " . $series;
				$curlURL = "http://weatherspot.us/db/query.php?u=weather_user&p=weather_password&db=" . $database . "&query=" . rawurlencode($q);
				$response = doCurl($curlURL);
				printFormatted($response, $displayFormat);
				break;
			}
		case 6:
			//Return with all points in a series for the given duration
			$database = $delimeters[1]; 
			$series = $delimeters[2]; 
			$point = $delimeters[3]; 
			$duration = $delimeters[4];

			dout("Database: " . $database . "<br><br>");
			dout("Series: " . $series . "<br><br>");
			dout("Points: " . $point . "<br><br>");
			dout("Duration: " . $duration . "<br><br>");
			if ($series == "all") {
				$series = "/.*/";
			}
			if ($point == "all") {
				$point = "*";
			}
			$q = "SELECT " . $point . " from " . $series . " WHERE " . $duration;
			$curlURL = "http://weatherspot.us/db/query.php?u=weather_user&p=weather_password&db=" . $database . "&query=" . rawurlencode($q);
			$response = doCurl($curlURL);
			printFormatted($response, $displayFormat);
			break;

			break;
	}
break;
case 'POST':
	//POST is used for creating resources
	//Example: create a new database, insert into a series
	dout("POST Not yet implemented");
break;
case 'PUT':
	//PUT is for changing state of a resource
	//Example: changing rows? migrating data?
	echo "PUT Not yet implemented";
break;
case 'DELETE':
	//DELETE is for removing resources
	//Example: drop tables, delete from series, rename series (currently unsupported)
	echo "DELETE Not yet implemented";
break;
}

function hexUrlEncode($url) {
	$enc = rawurlencode($url);
	$enc = str_replace("%26gt%3B", "%3e", $enc);	
	$enc = str_replace("%26lt%3B", "%3c", $enc);	
	return $enc;
}
function printFormatted($json, $format) {
	switch ($format) {
		case "xml":
			// Later: header('Content-Type: text/xml'); 
			$jsonArray = json_decode($json, TRUE);
			dout("Printing XML<br><br>");
			$xml = new SimpleXMLElement('<root/>');
			array_to_xml($jsonArray, $xml);
			echo htmlspecialchars($xml->asXML());
//			echo $xml->asXML();
			break;
		case "json":
			dout("Printing JSON<br><br>");
			echo $json;
			break;
		case "pretty":
			break;
		default:
			dout("Printing in default (JSON)<br><br>");
			echo $json;
			break;
	}
}

function array_to_xml($array, &$out_xml) {
//FROM: http://stackoverflow.com/questions/8830599/php-convert-xml-to-json
	foreach($array as $key => $value) {
		if(is_array($value)) {
			$key = is_numeric($key) ? "item$key" : $key;
			$subnode = $out_xml->addChild("$key");
			array_to_xml($value, $subnode);
		}
		else {
			$key = is_numeric($key) ? "item$key" : $key;
			$out_xml->addChild("$key","$value");
		}
	}
}

function doCurl($url) {
	$curl = curl_init();
	curl_setopt($curl, CURLOPT_URL, $url);
	curl_setopt($curl, CURLOPT_RETURNTRANSFER, TRUE);
	dout("Curling: " . $url . "<br>");
	$result = curl_exec($curl);
	curl_close($curl);

	return $result;
}

function tokenizeUrl($urlPath) {
	$delimeters = array();
	$token = strtok($urlPath, "/"); 
	$index = 0;
	while ($token !== false) {
		$delimeters[$index++] = $token;
		$token = strtok("/"); 
	}
	return $delimeters;
}

function get_current_url($strip = true) {
    // from http://stackoverflow.com/questions/14912943/how-to-print-current-url-path
    static $filter;
    $filter = function($input) use($strip) {
        $input = str_ireplace(array(
            "\0", '%00', "\x0a", '%0a', "\x1a", '%1a'), '', urldecode($input));
        if ($strip) {
            $input = strip_tags($input);
        }
        // or whatever encoding you use instead of utf-8
        $input = htmlentities($input, ENT_QUOTES, 'utf-8'); 
        return trim($input);
    };

    return 'http'. (($_SERVER['SERVER_PORT'] == '443') ? 's' : '')
        .'://'. $_SERVER['SERVER_NAME'] . $filter($_SERVER['REQUEST_URI']);
}

function dout($s) {
	$debug = FALSE;
	if ($debug == TRUE) {
		echo $s;
	}
}
?>
