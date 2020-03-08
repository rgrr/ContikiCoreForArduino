# ContikiCoreForArduino
Library containing some core components of [Contiki-OS](http://contiki-os.org/).

## Components
Contained are a few Contiki core components.  Those are `protothread`s, `process`es, `timer`s and `etimer`s.

The actual scheduling of the processes has to be done in the Arduino main loop, see example.


## What's missing?
* the scheduling loop is actually polling which should not be the case
  * instead scheduling should timer interrupt triggered
  * in between scheduling event device should go low power
* release notes which appear at the porper place in pio


## Release Notes
### 0.0.7 (08.03.2020)
* library is now a pio project on its own

### 0.0.6 (01.03.2020)
* initial release
