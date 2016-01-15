"use strict;"

var todoist_endpoint = 'https://todoist.com/API/v6/sync';
var ifttt_endpoint = 'https://maker.ifttt.com/trigger/';

function post(payload) {
  if (payload.ifttt_event === '') {
    return;
  }
  if (payload.ifttt_token === '') {
    console.log("IFTTT Maker Channel key is not available");
    return;
  }
  var req = new XMLHttpRequest();
  req.open('POST', ifttt_endpoint + payload.ifttt_event + '/with/key/' + payload.ifttt_token, true);
  req.setRequestHeader("Content-Type", "application/json");
  var data = {"value1": payload.title};
  req.onload = function () {
    if (req.readyState === 4 && req.status === 200) {
      console.log(req.responseText);
    }
    else {
      console.log("fetch failed with " + req.status + ": " + req.statusText);
    }
  };
  req.send(JSON.stringify(data));
}

function fetch(todoist_token) {
  if (todoist_token === '') {
    return;
  }
  var req = new XMLHttpRequest();
  req.open('GET', todoist_endpoint + '?token=' + todoist_token + '&seq_no=0' + '&resource_types=' + encodeURIComponent('["items"]'), true);
  req.onload = function () {
    if (req.readyState === 4 && req.status === 200) {
      var response = JSON.parse(req.responseText);
      var elements = response.Items.sort(function(a, b) {
        return a.item_order < b.item_order ? -1 : 1;
      });
      var tomorrow = new Date();
      tomorrow.setDate(tomorrow.getDate() + 1);
      elements.forEach(function(e, i, a) {
        if (e.checked === 1 || e.is_archived === 1 || e.is_deleted === 1)
          return;
        if (e.due_date != null && new Date(e.due_date) <= tomorrow) {
          var id = e.id;
          var name = e.content;
          Pebble.sendAppMessage({
            'item_id': id,
            'item_name': name
          });
        }
      });
    }
    else {
      console.log("fetch failed with " + req.status + ": " + req.statusText);
    }
  };
  req.send(null);
}

Pebble.addEventListener('ready', function (e) {
  console.log('connected! ' + e.ready);
});

Pebble.addEventListener('appmessage', function (e) {
  if (e.payload.hasOwnProperty("todoist_token")) {
    fetch(e.payload.todoist_token);
  }
  if (e.payload.hasOwnProperty("ifttt_token")) {
    post(e.payload);
  }
});

Pebble.addEventListener("showConfiguration", function() {
  console.log("showing configuration");
  Pebble.openURL('http://pebble.turbare.net/limone-watchapp/configurable.html');
});

Pebble.addEventListener("webviewclosed", function(e) {
  console.log("configuration closed");
  options = JSON.parse(decodeURIComponent(e.response));
  Pebble.sendAppMessage(options);
});
