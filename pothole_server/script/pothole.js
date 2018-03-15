/* Dummy data for avoiding Google Maps for caching the file */
var src = 'https://rainmaker1110.000webhostapp.com/maps/potholes.json?dummy=' + (new Date()).getTime();;

var infoWindow;

/**
 * Initializes the map and calls the function that creates polylines.
 */
function initMap() {
	var map = new google.maps.Map(document.getElementById('map'), {
		center: new google.maps.LatLng(19.4331373, -99.1322891),
		zoom: 12,
		mapTypeId: 'roadmap'
	});

	infoWindow = new google.maps.InfoWindow();

	infoWindow.setOptions({pixelOffset: new google.maps.Size(0,-30)});

	map.data.loadGeoJson(src);

	map.data.setStyle({
		icon: 'https://rainmaker1110.000webhostapp.com/maps/img/skull.png'
	});

	map.data.addListener('click', function(event){
		var address = event.feature.getProperty('address');
		var contentHTML = '<p><strong>Direcci&oacute;n:</strong><br />';

		contentHTML += address.replace(/, /g, '<br />');
		contentHTML += '.</p>';
		contentHTML += '<a href="./remove_pothole.php?pothole_id=';
		contentHTML += event.feature.getProperty('id');
		contentHTML += '">Eliminar</a>';

		infoWindow.setContent(contentHTML);
		infoWindow.setPosition(event.feature.getGeometry().get());
		infoWindow.open(map);
	});
}
