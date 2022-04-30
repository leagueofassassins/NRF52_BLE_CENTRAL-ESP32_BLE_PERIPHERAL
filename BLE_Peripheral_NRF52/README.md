##Hardware Required

ESP32 dev board
LIS2DW12 module

##Menuconfig 

idf.py menuconfig

Component config->Bluetooth->Bluedroid Options
 
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
I (1034) Bluedroid-Peripheral: attribute table:44
I (1044) Bluedroid-Peripheral: attribute table:45
I (1044) Bluedroid-Peripheral: attribute table:46
I (1054) Bluedroid-Peripheral: attribute table:47
I (1054) Bluedroid-Peripheral: attribute table:48
I (1064) Bluedroid-Peripheral: attribute table:49
I (1074) Bluedroid-Peripheral: Mac address
I (1074) Bluedroid-Peripheral: 7c:
I (1074) Bluedroid-Peripheral: 9e:
I (1084) Bluedroid-Peripheral: bd:
I (1084) Bluedroid-Peripheral: 45:
I (1094) Bluedroid-Peripheral: 9a:
I (1094) Bluedroid-Peripheral: fc:
I (1114) Bluedroid-Peripheral: I2C initialized...................
I (1124) Bluedroid-Peripheral: advertising start success
I (1144) Bluedroid-Peripheral: advertising start success
I (1154) Bluedroid-Peripheral: advertising start success
I (1164) Bluedroid-Peripheral: LIS2DW12 validated................
I (1164) Bluedroid-Peripheral: WHO_AM_I:44
I (1164) Bluedroid-Peripheral: CTRL2:8
I (1164) Bluedroid-Peripheral: FIFO_CTRL:c0
I (1174) Bluedroid-Peripheral: CTRL1:20
I (1174) Bluedroid-Peripheral: CTRL3:1
I (68354) Bluedroid-Peripheral: ESP_GATTS_CONNECT_EVT
I (68834) Bluedroid-Peripheral: attribute handle:43, len:2
I (68834) Bluedroid-Peripheral: 01 00
I (68834) Bluedroid-Peripheral: notify enabled, handle:43
I (68844) Bluedroid-Peripheral: sent data:144.000000 for x-axes
I (68844) Bluedroid-Peripheral: conn_id:0
I (68924) Bluedroid-Peripheral: attribute handle:46, len:2
I (68924) Bluedroid-Peripheral: 01 00
I (68924) Bluedroid-Peripheral: notify enabled, handle:46
I (68934) Bluedroid-Peripheral: sent data:64.000000 for y-axes
I (68934) Bluedroid-Peripheral: conn_id:0
I (68984) Bluedroid-Peripheral: attribute handle:49, len:2
I (68984) Bluedroid-Peripheral: 01 00
I (68984) Bluedroid-Peripheral: notify enabled, handle:49
I (68994) Bluedroid-Peripheral: sent data:32.000000 for z-axes
I (68994) Bluedroid-Peripheral: conn_id:0
I (69174) Bluedroid-Peripheral: notify enabled
I (69184) Bluedroid-Peripheral: x(mg):128.000000
I (69184) Bluedroid-Peripheral: sent data:128.000000 for x-axes
I (69184) Bluedroid-Peripheral: conn_id:0
I (69194) Bluedroid-Peripheral: y(mg):16.000000
I (69194) Bluedroid-Peripheral: sent data:16.000000 for y-axes
I (69204) Bluedroid-Peripheral: conn_id:0
I (69204) Bluedroid-Peripheral: z(mg):128.000000
I (69214) Bluedroid-Peripheral: sent data:128.000000 for z-axes
I (69214) Bluedroid-Peripheral: conn_id:0
I (70234) Bluedroid-Peripheral: notify enabled
I (70234) Bluedroid-Peripheral: x(mg):208.000000
I (70244) Bluedroid-Peripheral: sent data:208.000000 for x-axes
I (70244) Bluedroid-Peripheral: conn_id:0
I (70244) Bluedroid-Peripheral: y(mg):64.000000
I (70254) Bluedroid-Peripheral: sent data:64.000000 for y-axes
I (70254) Bluedroid-Peripheral: conn_id:0
I (70264) Bluedroid-Peripheral: z(mg):32.000000
I (70274) Bluedroid-Peripheral: sent data:32.000000 for z-axes
I (70274) Bluedroid-Peripheral: conn_id:0
I (71284) Bluedroid-Peripheral: notify enabled
I (71284) Bluedroid-Peripheral: x(mg):0.000000
I (71284) Bluedroid-Peripheral: sent data:0.000000 for x-axes
I (71284) Bluedroid-Peripheral: conn_id:0
I (71294) Bluedroid-Peripheral: y(mg):64.000000
I (71294) Bluedroid-Peripheral: sent data:64.000000 for y-axes
I (71314) Bluedroid-Peripheral: conn_id:0
I (71314) Bluedroid-Peripheral: z(mg):80.000000
I (71314) Bluedroid-Peripheral: sent data:80.000000 for z-axes
I (71334) Bluedroid-Peripheral: conn_id:0
I (72334) Bluedroid-Peripheral: notify enabled
I (72334) Bluedroid-Peripheral: x(mg):160.000000
I (72344) Bluedroid-Peripheral: sent data:160.000000 for x-axes
I (72344) Bluedroid-Peripheral: conn_id:0
I (72344) Bluedroid-Peripheral: y(mg):64.000000
I (72354) Bluedroid-Peripheral: sent data:64.000000 for y-axes
I (72354) Bluedroid-Peripheral: conn_id:0
I (72364) Bluedroid-Peripheral: z(mg):0.000000
I (72364) Bluedroid-Peripheral: sent data:0.000000 for z-axes
I (72374) Bluedroid-Peripheral: conn_id:0
I (73384) Bluedroid-Peripheral: notify enabled
I (73384) Bluedroid-Peripheral: x(mg):144.000000
I (73384) Bluedroid-Peripheral: sent data:144.000000 for x-axes
I (73384) Bluedroid-Peripheral: conn_id:0
I (73394) Bluedroid-Peripheral: y(mg):0.000000
I (73394) Bluedroid-Peripheral: sent data:0.000000 for y-axes
I (73404) Bluedroid-Peripheral: conn_id:0
I (73404) Bluedroid-Peripheral: z(mg):32.000000
I (73414) Bluedroid-Peripheral: sent data:32.000000 for z-axes
I (73424) Bluedroid-Peripheral: conn_id:0
I (74434) Bluedroid-Peripheral: notify enabled
I (74434) Bluedroid-Peripheral: x(mg):16.000000
I (74434) Bluedroid-Peripheral: sent data:16.000000 for x-axes
I (74434) Bluedroid-Peripheral: conn_id:0
I (74444) Bluedroid-Peripheral: y(mg):240.000000
I (74444) Bluedroid-Peripheral: sent data:240.000000 for y-axes
I (74464) Bluedroid-Peripheral: conn_id:0
I (74464) Bluedroid-Peripheral: z(mg):48.000000
I (74474) Bluedroid-Peripheral: sent data:48.000000 for z-axes
I (74474) Bluedroid-Peripheral: conn_id:0
I (75484) Bluedroid-Peripheral: notify enabled
I (75484) Bluedroid-Peripheral: x(mg):160.000000
I (75484) Bluedroid-Peripheral: sent data:160.000000 for x-axes
I (75484) Bluedroid-Peripheral: conn_id:0
I (75494) Bluedroid-Peripheral: y(mg):240.000000
I (75494) Bluedroid-Peripheral: sent data:240.000000 for y-axes
I (75514) Bluedroid-Peripheral: conn_id:0
I (75514) Bluedroid-Peripheral: z(mg):0.000000
I (75514) Bluedroid-Peripheral: sent data:0.000000 for z-axes
I (75534) Bluedroid-Peripheral: conn_id:0
I (76534) Bluedroid-Peripheral: notify enabled
I (76534) Bluedroid-Peripheral: x(mg):192.000000
I (76544) Bluedroid-Peripheral: sent data:192.000000 for x-axes
I (76544) Bluedroid-Peripheral: conn_id:0
I (76544) Bluedroid-Peripheral: y(mg):160.000000
I (76554) Bluedroid-Peripheral: sent data:160.000000 for y-axes
I (76554) Bluedroid-Peripheral: conn_id:0
I (76564) Bluedroid-Peripheral: z(mg):64.000000
I (76564) Bluedroid-Peripheral: sent data:64.000000 for z-axes
I (76574) Bluedroid-Peripheral: conn_id:0
I (77584) Bluedroid-Peripheral: notify enabled
I (77584) Bluedroid-Peripheral: x(mg):64.000000
I (77584) Bluedroid-Peripheral: sent data:64.000000 for x-axes
I (77584) Bluedroid-Peripheral: conn_id:0
I (77594) Bluedroid-Peripheral: y(mg):16.000000
I (77594) Bluedroid-Peripheral: sent data:16.000000 for y-axes
I (77614) Bluedroid-Peripheral: conn_id:0
I (77614) Bluedroid-Peripheral: z(mg):208.000000
I (77614) Bluedroid-Peripheral: sent data:208.000000 for z-axes
I (77634) Bluedroid-Peripheral: conn_id:0
I (78634) Bluedroid-Peripheral: notify enabled
I (78634) Bluedroid-Peripheral: x(mg):0.000000
I (78644) Bluedroid-Peripheral: sent data:0.000000 for x-axes
I (78644) Bluedroid-Peripheral: conn_id:0
I (78644) Bluedroid-Peripheral: y(mg):224.000000
I (78654) Bluedroid-Peripheral: sent data:224.000000 for y-axes
I (78654) Bluedroid-Peripheral: conn_id:0
I (78664) Bluedroid-Peripheral: z(mg):96.000000
I (78674) Bluedroid-Peripheral: sent data:96.000000 for z-axes
I (78674) Bluedroid-Peripheral: conn_id:0