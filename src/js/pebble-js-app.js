var token = '';
var endpoint = 'https://todoist.com/API/v6/sync';

function fetch() {
  var req = new XMLHttpRequest();
  req.open('GET', endpoint + '?token=' + token + '&seq_no=0' + '&resource_types=' + encodeURIComponent('["items"]'), true);
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
            'ITEM_ID': id,
            'ITEM_NAME': name
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
  console.log("message:" + JSON.stringify(e.payload));
  if (e.payload.hasOwnProperty("fetch_items")) {
    fetch();
  }
});
