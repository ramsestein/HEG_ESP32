const char api[] PROGMEM = R"=====(
<!-- To start, while connected to your device's access point try "var s = new HEGwebAPI('test', true, 'http://192.168.4.1')" in your console and see what happens! -->
<link rel="stylesheet" type="text/css" href="webDemoCSS.css">
<script src="HEGwebAPI.js"></script>
<!-- Refer to connect.html and update.html for correct POST format to control connection settings and updates. -->
<!-- To return scan results for WiFi, you must GET /scan at the device IP -->
<div id="test"></div>
)=====";