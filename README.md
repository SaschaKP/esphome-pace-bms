# esphome-pace-bms

ESPHome component to monitor a Pace BMS via UART or RS485

## Supported devices

* DEYE (from VTAC -> VT-12040 200Ah 51.2V battery - PACE BMS)
* POWMR (POW-LIO48200-16S)
  ```
  protocol_version: 0x25
  override_pack: 1
  ```

## Untested devices

* Any other PACE BMS with 2.5 protocol version

## Schematics

```
                  RS232                      UART
┌────────────┐              ┌──────────┐                ┌─────────┐
│            │              │          │<----- RX ----->│         │
│    Deye    │<-----rx ---->│  RS232   │<----- TX ----->│ ESP32/  │
│    BMS     │<---- tx ---->│  to TTL  │<----- GND ---->│ ESP8266 │
│            │<--- GND ---->│  module  │<----- 3.3V --->│         │<-- VCC
│            │              │          │                │         │<-- GND
└────────────┘              └──────────┘                └─────────┘

```

Please be aware of the different RJ11 pinout

Also consider that if you're using the VTAC from Deye, they should have given you a wifi module.
That specific module can be reprogrammed and used with my software, upon opening it there are the pinouts
stamped on the board, that will allow you to rewrite firmware, it should use a ESP32-S device, that will 
be inserted into the ESI port of the deye inverter (External Serial Interface).

## Requirements

* [ESPHome 2024.6.0 or higher](https://github.com/esphome/esphome/releases).
* Generic ESP32 or ESP8266 board

## Installation

You can install this component with [ESPHome external components feature](https://esphome.io/components/external_components.html) like this:
```yaml
external_components:
  - source: github://SaschaKP/esphome-pace-bms@main
```

or just use the `esp32-example.yaml` as proof of concept:

```bash
# Install esphome
pip3 install esphome

# Clone this external component
git clone https://github.com/SaschaKP/esphome-pace-bms.git
cd esphome-pace-bms

# Create a secrets.yaml containing some setup specific secrets
cat > secrets.yaml <<EOF
wifi_ssid: MY_WIFI_SSID
wifi_password: MY_WIFI_PASSWORD

mqtt_host: MY_MQTT_HOST
mqtt_username: MY_MQTT_USERNAME
mqtt_password: MY_MQTT_PASSWORD
EOF

# Validate the configuration, create a binary, upload it, and start logs
# If you use a esp8266 run the esp8266-examle.yaml
esphome run esp32-example.yaml
```

## Example response all sensors enabled

```
[21:27:56][I][pace_bms:034]: Telemetry frame (68 bytes) received
[21:27:56][D][sensor:103]: 'Bank 1 cell voltage 1': Sending state 3.31800 V with 3 decimals of accuracy
[21:27:56][D][sensor:103]: 'Bank 1 cell voltage 2': Sending state 3.32000 V with 3 decimals of accuracy
[21:27:56][D][sensor:103]: 'Bank 1 cell voltage 3': Sending state 3.31900 V with 3 decimals of accuracy
[21:27:56][D][sensor:103]: 'Bank 1 cell voltage 4': Sending state 3.31900 V with 3 decimals of accuracy
[21:27:56][D][sensor:103]: 'Bank 1 cell voltage 5': Sending state 3.31900 V with 3 decimals of accuracy
[21:27:56][D][sensor:103]: 'Bank 1 cell voltage 6': Sending state 3.31900 V with 3 decimals of accuracy
[21:27:56][D][sensor:103]: 'Bank 1 cell voltage 7': Sending state 3.31900 V with 3 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 cell voltage 8': Sending state 3.31800 V with 3 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 cell voltage 9': Sending state 3.31800 V with 3 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 cell voltage 10': Sending state 3.31900 V with 3 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 cell voltage 11': Sending state 3.31900 V with 3 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 cell voltage 12': Sending state 3.31800 V with 3 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 cell voltage 13': Sending state 3.32000 V with 3 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 cell voltage 14': Sending state 3.31900 V with 3 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 cell voltage 15': Sending state 3.31900 V with 3 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 cell voltage 16': Sending state 3.31900 V with 3 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 min cell voltage': Sending state 3.31800 V with 3 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 max cell voltage': Sending state 3.32000 V with 3 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 max voltage cell': Sending state 2.00000  with 0 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 min voltage cell': Sending state 1.00000  with 0 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 delta cell voltage': Sending state 0.00200 V with 3 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 average cell voltage': Sending state 3.31888 V with 3 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 temperature 1': Sending state 29.20000 °C with 0 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 temperature 2': Sending state 27.70000 °C with 0 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 temperature 3': Sending state 29.20000 °C with 0 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 temperature 4': Sending state 27.80000 °C with 0 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 mosfet temperature': Sending state 29.40000 °C with 0 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 environment temperature': Sending state 29.20000 °C with 0 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 current': Sending state -7.13000 A with 2 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 total voltage': Sending state 53.10200 V with 2 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 power': Sending state -378.61725 W with 2 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 charging power': Sending state 0.00000 W with 2 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 discharging power': Sending state 378.61725 W with 2 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 residual capacity': Sending state 180.75999 Ah with 2 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 battery capacity': Sending state 199.84000 Ah with 2 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 charging cycles': Sending state 194.00000  with 0 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 rated capacity': Sending state 200.00000 Ah with 2 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 state of charge': Sending state 90.45236 % with 1 decimals of accuracy
[21:27:57][D][sensor:103]: 'Bank 1 state of health': Sending state 99.92000 % with 1 decimals of accuracy

[21:28:01][I][pace_bms:034]: Telemetry frame (68 bytes) received
[21:28:01][D][sensor:103]: 'Bank 2 cell voltage 1': Sending state 3.31600 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 cell voltage 2': Sending state 3.31800 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 cell voltage 3': Sending state 3.31800 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 cell voltage 4': Sending state 3.31800 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 cell voltage 5': Sending state 3.31800 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 cell voltage 6': Sending state 3.31800 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 cell voltage 7': Sending state 3.31800 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 cell voltage 8': Sending state 3.31800 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 cell voltage 9': Sending state 3.31800 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 cell voltage 10': Sending state 3.31800 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 cell voltage 11': Sending state 3.31800 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 cell voltage 12': Sending state 3.31800 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 cell voltage 13': Sending state 3.31800 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 cell voltage 14': Sending state 3.31800 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 cell voltage 15': Sending state 3.31800 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 cell voltage 16': Sending state 3.31600 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 min cell voltage': Sending state 3.31600 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 max cell voltage': Sending state 3.31800 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 max voltage cell': Sending state 2.00000  with 0 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 min voltage cell': Sending state 1.00000  with 0 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 delta cell voltage': Sending state 0.00200 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 average cell voltage': Sending state 3.31775 V with 3 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 temperature 1': Sending state 27.70000 °C with 0 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 temperature 2': Sending state 28.40000 °C with 0 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 temperature 3': Sending state 28.70000 °C with 0 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 temperature 4': Sending state 28.70000 °C with 0 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 mosfet temperature': Sending state 25.10000 °C with 0 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 environment temperature': Sending state 26.90000 °C with 0 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 current': Sending state -5.18000 A with 2 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 total voltage': Sending state 53.17400 V with 2 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 power': Sending state -275.44131 W with 2 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 charging power': Sending state 0.00000 W with 2 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 discharging power': Sending state 275.44131 W with 2 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 residual capacity': Sending state 187.89999 Ah with 2 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 battery capacity': Sending state 207.00000 Ah with 2 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 charging cycles': Sending state 8.00000  with 0 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 rated capacity': Sending state 200.00000 Ah with 2 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 state of charge': Sending state 90.77294 % with 1 decimals of accuracy
[21:28:01][D][sensor:103]: 'Bank 2 state of health': Sending state 100.00000 % with 1 decimals of accuracy
```

## Known issues and limitations

None.

## Protocol

PACE RS232 protocol is really a mess, but anyway you can find sufficient code and docs in: https://github.com/nkinnan/esphome-pace-bms from whom I read and understood some code, and
also from inspecting the code with a c# decompiler with tools like Jetbrain_dotpeek, with whom I read the specific parts necessary to retrieve correctly the data from my BMSs

```
$ echo -ne "~20004642E00200FD37\r" | hexdump -ve '1/1 "%.2X."'
      7E.32.30.30.30.34.36.34.32.45.30.30.32.30.30.46.44.33.37.0D.

# Get pack #1 telemetry data (CID2 `0x42` - from SaschaKP git)
TX -> "~25014642E00201FD30\r"
RX <- "~25014600F07A0001100CC70CC80CC70CC70CC70CC50CC60CC70CC70CC60CC70CC60CC60CC70CC60CC7060B9B0B990B990B990BB30BBCFF1FCCCD12D303286A008C2710E1E4\r"

```

## Debugging

If this component doesn't work out of the box for your device please update your configuration to enable the debug output of the UART component and increase the log level to the see outgoing and incoming serial traffic:

```
logger:
  level: VERY_VERBOSE

uart:
  id: uart_0
  baud_rate: 9600
  tx_pin: ${tx_pin}
  rx_pin: ${rx_pin}
  rx_buffer_size: 1024
  debug:
    dummy_receiver: false
    direction: BOTH
    after:
      delimiter: "\r"
    sequence:
      - lambda: UARTDebug::log_string(direction, bytes);
```

## References

* https://github.com/nkinnan/esphome-pace-bms
