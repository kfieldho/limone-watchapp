"use strict;"

var options = {
  "todoist_token": "",
  "ifttt_token": "",
  "ifttt_started": "",
  "ifttt_canceled": "",
  "ifttt_finished": "",
  "from_fmt": "%Y-%m-%dT%H:%M:%S%Z",
  "to_fmt": "%Y-%m-%dT%H:%M:%S%Z"
}

var todoist_endpoint = 'https://todoist.com/API/v6/sync';
var ifttt_endpoint = 'https://maker.ifttt.com/trigger/';

// some modification from public domain software http://dren.ch/strftime/
Number.prototype.pad = function (n,p) {
	var s = '' + this;
	p = p || '0';
	while (s.length < n) s = p + s;
	return s;
};
Date.prototype.months = ['January', 'February', 'March', 'April', 'May', 'June', 'July', 'August', 'September', 'October', 'November', 'December'];
Date.prototype.weekdays = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];
Date.prototype.dpm = [ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 ];
Date.prototype.strftime_f = {
	A: function (d) { return d.weekdays[d.getDay()] },
	a: function (d) { return d.weekdays[d.getDay()].substring(0,3) },
	B: function (d) { return d.months[d.getMonth()] },
	b: function (d) { return d.months[d.getMonth()].substring(0,3) },
	C: function (d) { return Math.floor(d.getFullYear()/100); },
	c: function (d) { return d.toString() },
	D: function (d) { return d.strftime_f.m(d) + '/' + d.strftime_f.d(d) + '/' + d.strftime_f.y(d); },
	d: function (d) { return d.getDate().pad(2,'0') },
	e: function (d) { return d.getDate().pad(2,' ') },
	F: function (d) { return d.strftime_f.Y(d) + '-' + d.strftime_f.m(d) + '-' + d.strftime_f.d(d); },
	H: function (d) { return d.getHours().pad(2,'0') },
	I: function (d) { return ((d.getHours() % 12 || 12).pad(2)) },
	j: function (d) {
		var t = d.getDate();
		var m = d.getMonth() - 1;
		if (m > 1) {
			var y = d.getYear();
			if (((y % 100) == 0) && ((y % 400) == 0)) ++t;
			else if ((y % 4) == 0) ++t;
		}
		while (m > -1) t += d.dpm[m--];
		return t.pad(3,'0');
	},
	k: function (d) { return d.getHours().pad(2,' ') },
	l: function (d) { return ((d.getHours() % 12 || 12).pad(2,' ')) },
	M: function (d) { return d.getMinutes().pad(2,'0') },
	m: function (d) { return (d.getMonth()+1).pad(2,'0') },
	n: function (d) { return "\n" },
	p: function (d) { return (d.getHours() > 11) ? 'PM' : 'AM' },
	R: function (d) { return d.strftime_f.H(d) + ':' + d.strftime_f.M(d) },
	r: function (d) { return d.strftime_f.I(d) + ':' + d.strftime_f.M(d) + ':' + d.strftime_f.S(d) + ' ' + d.strftime_f.p(d); },
	S: function (d) { return d.getSeconds().pad(2,'0') },
	s: function (d) { return Math.floor(d.getTime()/1000) },
	T: function (d) { return d.strftime_f.H(d) + ':' + d.strftime_f.M(d) + ':' + d.strftime_f.S(d); },
	t: function (d) { return "\t" },
	u: function (d) { return(d.getDay() || 7) },
	v: function (d) { return d.strftime_f.e(d) + '-' + d.strftime_f.b(d) + '-' + d.strftime_f.Y(d); },
	w: function (d) { return d.getDay() },
	X: function (d) { return d.toTimeString() }, // wrong?
	x: function (d) { return d.toDateString() }, // wrong?
	Y: function (d) { return d.getFullYear() },
	y: function (d) { return (d.getYear() % 100).pad(2) },
  z: function (d) {
    var t = d.getTimezoneOffset();
    var s = t > 0 ? '-' : '+';
    t = t < 0 ? -t : t;
    return s + (t / 60).pad(2,'0') + ':' + (t % 60).pad(2,'0');
  },
	'%': function (d) { return '%' }
};
Date.prototype.strftime_f['+'] = Date.prototype.strftime_f.c;
Date.prototype.strftime_f.h = Date.prototype.strftime_f.b;
Date.prototype.strftime = function (fmt) {
	var r = '';
	var n = 0;
	while(n < fmt.length) {
		var c = fmt.substring(n, n+1);
		if (c == '%') {
			c = fmt.substring(++n, n+1);
			r += (this.strftime_f[c]) ? this.strftime_f[c](this) : c;
		} else r += c;
		++n;
	}
	return r;
};

function post(payload) {
  var eventName;
  if (payload.hasOwnProperty("started")) {
    eventName = options.ifttt_started;
  }
  else if (payload.hasOwnProperty("canceled")) {
    eventName = options.ifttt_canceled;
  }
  else if (payload.hasOwnProperty("finished")) {
    eventName = options.ifttt_finished;
  }
  if (eventName === '') {
    return;
  }
  if (options.ifttt_token === '') {
    console.log("IFTTT Maker Channel key is not available");
    return;
  }
  var req = new XMLHttpRequest();
  req.open('POST', ifttt_endpoint + eventName + '/with/key/' + options.ifttt_token, true);
  req.setRequestHeader("Content-Type", "application/json");
  var from = new Date(payload.from * 1000).strftime(options.from_fmt);
  var to = payload.to > 0 ? new Date(payload.to * 1000).strftime(options.to_fmt) : "";
  var data = {"value1": payload.title, "value2": from, "value3": to};
  req.send(JSON.stringify(data));
}

function fetch() {
  if (options.todoist_token === '') {
    console.log("Todoist API token is not available");
    return;
  }
  var req = new XMLHttpRequest();
  req.open('GET', todoist_endpoint + '?token=' + options.todoist_token + '&seq_no=0' + '&resource_types=' + encodeURIComponent('["items"]'), true);
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
  if (e.payload.hasOwnProperty("fetch_items")) {
    fetch();
  }
  if (e.payload.hasOwnProperty("started") ||
      e.payload.hasOwnProperty("canceled") ||
      e.payload.hasOwnProperty("finished")) {
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
  console.log('Config window returned: ', JSON.stringify(options));
});
