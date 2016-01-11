"use strict;"

var todoist_token = '';
var todoist_endpoint = 'https://todoist.com/API/v6/sync';

var ifttt_token = '';
var ifttt_event = ''
var ifttt_endpoint = 'https://maker.ifttt.com/trigger/';
var start_fmt = "%Y-%m-%dT%H:%M:%S%z";
var end_fmt = "%Y-%m-%dT%H:%M:%S%z";

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
  var req = new XMLHttpRequest();
  req.open('POST', ifttt_endpoint + ifttt_event + '/with/key/' + ifttt_token, true);
  req.setRequestHeader("Content-Type", "application/json");
  var started = new Date(payload.STARTED * 1000);
  var ended = new Date(payload.ENDED * 1000);
  var data = {"value1": payload.TITLE, "value2": started.strftime(start_fmt), "value3": ended.strftime(end_fmt)};
  req.send(JSON.stringify(data));
}

function fetch() {
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
  if (e.payload.hasOwnProperty("FETCH_ITEMS")) {
    fetch();
  }
  if (e.payload.hasOwnProperty("TITLE")) {
    post(e.payload);
  }
});
