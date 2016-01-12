# Limone watchapp

![Version](https://img.shields.io/github/tag/uchida/limone-watchapp.svg)
[![License](https://img.shields.io/github/license/uchida/limone-watchapp.svg)](https://tldrlegal.com/license/creative-commons-cc0-1.0-universal)
[![CircleCI](https://img.shields.io/circleci/project/uchida/limone-watchapp.svg)](https://circleci.com/gh/uchida/limone-watchapp)

## Description

This is a simple pomodoro timer for Pebble,
to notify 25 minutes work finished and after take 5 minutes break.

About Pomodoro, see [The Pomodoro Technique](http://pomodorotechnique.com/).

If you want to record your work or to notify your state working/breaking, 
Limone watchapp has feature to trigger [IFTTT Maker Channel](https://ifttt.com/maker).

With proper configuration, this feature send a web request to [IFTTT Maker Channel](https://ifttt.com/maker) 
with EventName (start/cancel/finish), Value1 task name,
Value2 started time, Value3 finished or canceled time.

|Ingredients|Example                        |Notes                                                           |
|-----------|-------------------------------|----------------------------------------------------------------|
|EventName  |task_started                   |eventname for started/canceled/finished depends on configuration|
|Value1     |task                           |task name enable to select from Today's Todoist task            |
|Value2     |2016-01-12T20:17:23+00:00-07:00|the time when task is started, enable to change format          |
|Value3     |2016-01-12T20:17:23+00:00-07:00|the time when task is canceled/finished, enable to change format|

You can select task name from Today's tasks in [Todoist](https://todoist.com/)
with Todoist API token in configuration.

screenshots for work/break timer in Aplite, Basalt, Chalk are following.

![work timer for aplite](screenshots/aplite-screenshot1.png)
![work timer for basalt](screenshots/basalt-screenshot1.png)
![work timer for chalk](screenshots/chalk-screenshot1.png)

![break timer for aplite](screenshots/aplite-screenshot2.png)
![break timer for basalt](screenshots/basalt-screenshot2.png)
![break timer for chalk](screenshots/chalk-screenshot2.png)

and screenshots for task select menu in Aplite, Basalt, Chalk

![task select menu for aplite](screenshots/aplite-screenshot3.png)
![task select menu for basalt](screenshots/basalt-screenshot3.png)
![task select menu for chalk](screenshots/chalk-screenshot3.png)

The pebble watchapp name is Limone, because I started pomodoro technique
with a Lemon kitchen timer, not a Tomato one.

## Installation

Limone watchapp is available from [Pebble Appstore](https://apps.getpebble.com/en_US/application/569061bfd5ba00104e000016)

Watchapp PBW files, including latest version unreleased to appstore, or past versions, are 
available from [Github Release](https://github.com/uchida/limone-watchapp).
However, using these version are not recommended, install these versions at your own risk.

## License

dedicated to [![CC0](http://i.creativecommons.org/p/zero/1.0/80x15.png "CC0")](https://creativecommons.org/publicdomain/zero/1.0/).
