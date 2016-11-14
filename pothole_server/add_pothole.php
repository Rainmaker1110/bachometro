<?php

$JSON_FILE = './potholes.json';

$API_KEY = 'AIzaSyDGGCfmIk6szRTBCCEAmESo2Vl11GG0s4U';

function getAddress($lat, $lng) {
	global $API_KEY;

	$json = file_get_contents("https://maps.googleapis.com/maps/api/geocode/json?latlng=$lat,$lng&" . $API_KEY);
	$coordsData = json_decode($json, true);

	return $coordsData['results'][0]['formatted_address'];
}

function createGeoJSONFile($lat, $lng, $address) {
	global $JSON_FILE;

	$json = file_get_contents($JSON_FILE);

	$geojson = json_decode($json, true);

	$properties = array('id' => $lng . ',' . $lat, 'address' => $address);

	$coordinates = array((float) $lng, (float) $lat);

	$geometry = array('type' => 'Point', 'coordinates' => $coordinates);

	$geojson['features'][] = array('type' => 'Feature', 'properties' => $properties, 'geometry' => $geometry);

	return file_put_contents($JSON_FILE, json_encode($geojson));
}

if ($_POST['new_pothole']) {
	$address = getAddress($_POST["lat"], $_POST["lng"]);
	
	if (createGeoJSONFile($_POST["lat"], $_POST["lng"], $address)) {
		echo 'OK';
	} else {
		echo 'ERROR';
	}
} else {
	echo 'NOT RECEIVED ANYTHING';
}

?>
