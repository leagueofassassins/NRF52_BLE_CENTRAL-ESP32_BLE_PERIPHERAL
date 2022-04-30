##Hardware Required

ESP32 dev board

LIS2DW12 module

##Build, Flash and Monitor 

idf.py build

Replace PORT with serial port name:

idf.py -p PORT flash 

idf.py -p PORT monitor

(To exit the serial monitor, type Ctrl-])


## Example output
I (904) Bluedroid-Peripheral: app_main init bluetooth
I (994) Bluedroid-Peripheral: Mac address
I (994) Bluedroid-Peripheral: 7c:
I (994) Bluedroid-Peripheral: 9e:
I (994) Bluedroid-Peripheral: bd:
I (994) Bluedroid-Peripheral: 45:
I (1004) Bluedroid-Peripheral: 9a:
I (1004) Bluedroid-Peripheral: fc:
I (1024) Bluedroid-Peripheral: The number handle = a
I (1024) Bluedroid-Peripheral: attribute table:40
I (1024) Bluedroid-Peripheral: attribute table:41
I (1024) Bluedroid-Peripheral: attribute table:42
I (1034) Bluedroid-Peripheral: attribute table:43
I (1044) Bluedroid-Peripheral: attribute table:44
I (1044) Bluedroid-Peripheral: attribute table:45
I (1054) Bluedroid-Peripheral: attribute table:46
I (1054) Bluedroid-Peripheral: attribute table:47
I (1064) Bluedroid-Peripheral: attribute table:48
I (1064) Bluedroid-Peripheral: attribute table:49
I (1074) Bluedroid-Peripheral: Mac address
I (1074) Bluedroid-Peripheral: 7c:
I (1084) Bluedroid-Peripheral: 9e:
I (1084) Bluedroid-Peripheral: bd:
I (1084) Bluedroid-Peripheral: 45:
I (1094) Bluedroid-Peripheral: 9a:
I (1094) Bluedroid-Peripheral: fc:
I (1134) Bluedroid-Peripheral: advertising start success
I (1114) Bluedroid-Peripheral: I2C initialized...................
I (1144) Bluedroid-Peripheral: advertising start success
I (1144) Bluedroid-Peripheral: LIS2DW12 validated................
I (1154) Bluedroid-Peripheral: WHO_AM_I:44
I (1154) Bluedroid-Peripheral: advertising start success
I (1164) Bluedroid-Peripheral: CTRL2:8
I (1164) Bluedroid-Peripheral: FIFO_CTRL:c0
I (1164) Bluedroid-Peripheral: CTRL1:20
I (1174) Bluedroid-Peripheral: CTRL3:1
I (21584) Bluedroid-Peripheral: ESP_GATTS_CONNECT_EVT
I (37034) Bluedroid-Peripheral: attribute handle:43, len:2
I (37034) Bluedroid-Peripheral: 01 00
I (37034) Bluedroid-Peripheral: notify enabled, handle:43
I (37044) Bluedroid-Peripheral: sent data:160.000000 for x-axes
I (37044) Bluedroid-Peripheral: conn_id:0
I (37174) Bluedroid-Peripheral: notify enabled
I (37174) Bluedroid-Peripheral: x(mg):192.000000
I (37174) Bluedroid-Peripheral: sent data:192.000000 for x-axes
I (37184) Bluedroid-Peripheral: conn_id:0
I (38194) Bluedroid-Peripheral: notify enabled
I (38194) Bluedroid-Peripheral: x(mg):192.000000
I (38194) Bluedroid-Peripheral: sent data:192.000000 for x-axes
I (38194) Bluedroid-Peripheral: conn_id:0
I (38204) Bluedroid-Peripheral: attribute handle:43, len:2
I (38204) Bluedroid-Peripheral: 00 00
I (38214) Bluedroid-Peripheral: notify disabled, handle:43
I (41814) Bluedroid-Peripheral: attribute handle:46, len:2
I (41814) Bluedroid-Peripheral: 01 00
I (41814) Bluedroid-Peripheral: notify enabled, handle:46
I (41814) Bluedroid-Peripheral: sent data:64.000000 for y-axes
I (41824) Bluedroid-Peripheral: conn_id:0
I (42234) Bluedroid-Peripheral: y(mg):144.000000
I (42234) Bluedroid-Peripheral: sent data:144.000000 for y-axes
I (42234) Bluedroid-Peripheral: conn_id:0
I (43074) Bluedroid-Peripheral: attribute handle:46, len:2
I (43084) Bluedroid-Peripheral: 00 00
I (43084) Bluedroid-Peripheral: notify disabled, handle:43
I (44934) Bluedroid-Peripheral: attribute handle:49, len:2
I (44934) Bluedroid-Peripheral: 01 00
I (44934) Bluedroid-Peripheral: notify enabled, handle:49
I (44944) Bluedroid-Peripheral: sent data:32.000000 for z-axes
I (44944) Bluedroid-Peripheral: conn_id:0
I (45254) Bluedroid-Peripheral: z(mg):0.000000
I (45254) Bluedroid-Peripheral: sent data:0.000000 for z-axes
I (45254) Bluedroid-Peripheral: conn_id:0
I (46264) Bluedroid-Peripheral: z(mg):240.000000
I (46264) Bluedroid-Peripheral: sent data:240.000000 for z-axes
I (46264) Bluedroid-Peripheral: conn_id:0
I (46494) Bluedroid-Peripheral: attribute handle:49, len:2
I (46494) Bluedroid-Peripheral: 00 00
I (46494) Bluedroid-Peripheral: notify disabled, handle:43