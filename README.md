# ContikiCoreForArduino
Library containing some core components of [Contiki-OS](http://contiki-os.org/) for ESP32 and RP2040.

## Components
Contained are a few Contiki core components.  Those are `protothread`s, `process`es, `timer`s and `etimer`s.

The actual scheduling of the processes has to be done in the Arduino main loop, see example.


## What's missing?
* the scheduling loop is actually polling which should not be the case
  * scheduling should be timer interrupt triggered
  * device should go to low power between scheduling events
* release notes which appear at the proper place in pio
* generation of build configuration depending on CPU, currently this is done via ifdefs which is not the nice way...


## Release Notes
### 0.0.8 (2022-11-23)
* new target: RP2040

### 0.0.7 (2020-03-08)
* library is now a pio project on its own

### 0.0.6 (2020-03-01)
* initial release
