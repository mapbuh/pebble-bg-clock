var initialized = false;

Pebble.addEventListener("ready", function() {
  console.log("ready called!");
  initialized = true;
});

Pebble.addEventListener("showConfiguration", function() {
  console.log("showing configuration");
  Pebble.openURL('http://docho.alphons.org/pebble-fuzzy_time_bg.html');
});


Pebble.addEventListener("webviewclosed", function(e) {
  console.log("configuration closed");
  // webview closed
  var configData = JSON.parse(decodeURIComponent(e.response));
  var dict = {
    'AppKeyTimeFormat': configData.timeFormat,
    'AppKeyDateFormat': configData.dateFormat
  };
  console.log("Options = " + JSON.stringify(configData));
  Pebble.sendAppMessage(dict, function() {
    console.log('Config data sent successfully!');
  }, function(e) {
    console.log('Error sending config data!');
  });
});
