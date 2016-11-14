<?php

$JSON_FILE = './potholes.json';

function removeFromFile($id) {
	global $JSON_FILE;

	$json = file_get_contents($JSON_FILE);

	$geojson = json_decode($json, true);

	foreach ($geojson['features'] as $key => $value) {
		if ($value['properties']['id'] == $_GET['pothole_id']) {
			array_splice($geojson['features'], $key, 1);
		}
	}

	file_put_contents($JSON_FILE, json_encode($geojson));
}

if (isset($_GET['pothole_id'])) {
	removeFromFile($_GET['pothole_id']);
}

header('Location: ./');

?>
