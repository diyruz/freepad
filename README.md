# Freepad

Freepad is an open source Zigbee remote intended to be used to have a customizable keypad to control your smart home devices.  

There are different hardware versions of the console.
More details on the links:
* [Freepad v1](https://github.com/diyruz/freepad/README_FREEPAD_v1.md)
* [Freepad v2](https://github.com/diyruz/freepad/README_FREEPAD_v2.md)
* [LeTV mod](https://github.com/diyruz/freepad/README_FREEPAD_LETV.md)

## Features list:
1. Single/double/tripple/quadriple/many_x/hold&release
2. Touchlink reset
3. ONOFF bind
4. Level control bind
5. Bindings configuration
6. Remote reset

## How to join:
1. Press and hold the *first* button for 3-10 seconds (depending on whether the device is connected to the network) until the LED on the device flashes.
2. Wait, if the connection is successful, the device will blink 5 times.
3. If the connection fails, the device will blink 3 times.

***Find button mapping at project page***

### Support
The keypad is supported in:
* zigbee2mqtt via int & ext converter
* ioBroker
* SLS Gateway

## Settings
In zigbee2mqtt you could change setting at Exposes tab.
![](/images/z2m_exposes.png)

### Work modes
By default remote works as custom switch, with multiple clicks, but this behavior has own drawback.
In order to detect multiple clicks, remote sends commands with 300ms delay.
You can change this behavior by cost of double/triple/etc clicks.

To do that you need to change

`ZCL_CLUSTER_ID_GEN_ON_OFF_SWITCH_CONFIG` cluster `ATTRID_ON_OFF_SWITCH_TYPE` attribute

Values are:

`ON_OFF_SWITCH_TYPE_MOMENTARY` (0) -> no delay, but no multiple clicks, only single

```bash
mosquitto_pub -t "zigbee2mqtt/FN/BUTTON_NUM/set/switch_type" -m '0'
```

`ON_OFF_SWITCH_TYPE_MULTIFUNCTION` (2) -> 300ms delay, full set of clicks

```bash
mosquitto_pub -t "zigbee2mqtt/FN/BUTTON_NUM/set/switch_type" -m '2'
```


### ONOFF cluster binding
By default command is TOGGLE, but you can change this behavior.

Change `ZCL_CLUSTER_ID_GEN_ON_OFF_SWITCH_CONFIG` clusters attribute `ATTRID_ON_OFF_SWITCH_ACTIONS`

`ON_OFF_SWITCH_ACTIONS_ON`

```bash
mosquitto_pub -t "zigbee2mqtt/FN/BUTTON_NUM/set/switch_actions" -m '0'
```

`ON_OFF_SWITCH_ACTIONS_OFF`

```bash
mosquitto_pub -t "zigbee2mqtt/FN/BUTTON_NUM/set/switch_actions" -m '1'
```

`ON_OFF_SWITCH_ACTIONS_TOGGLE` (default value)

```bash
mosquitto_pub -t "zigbee2mqtt/FN/BUTTON_NUM/set/switch_actions" -m '2'
```

### Video
[@Vlad_Kolobov](http://t.me/@MoNo320) recorded a video showing the speed in two modes
[![video_speed_compare](https://img.youtube.com/vi/XvhNs9Tn12M/0.jpg)](https://www.youtube.com/watch?v=XvhNs9Tn12M)


### Acronyms in firmware file names

TL - touchlink

PM3 - Power Mode 3 (External Interrupts): 0.4 μA
