-> soft device version is 7.2.0<br />
-> To build project go to "nrf_esp/pca10059/s140/armgcc" and run make all command.<br />
-> hex file of the project will be generated in "nrf_esp/pca10059/s140/armgcc/_build/" folder.<br />
-> Flash Both .hex named s140_nrf52_7.2.0_softdevice.hex and nrf52840_xxaa.hex<br />

Hardware Required:<br />

NRF52840 Dongle<br />
USB to TTL Board<br />

To Print NRF_LOG:<br />

-> In sdk_config.h enable uart<br />
 &nbsp;#ifndef NRF_LOG_BACKEND_UART_ENABLED<br />
 &nbsp;#define NRF_LOG_BACKEND_UART_ENABLED 1<br />
 &nbsp;#endif<br />
-> In sdk_config.h add uart TX pin in my case it is pin 0.13<br />
&nbsp;#ifndef NRF_LOG_BACKEND_UART_TX_PIN<br />
&nbsp;#define NRF_LOG_BACKEND_UART_TX_PIN 13<br />
&nbsp;#endif<br />
-> connect TX pin and Groud(GND) pin of nrf52840 to the usb to ttl board<br />	

Example Output:<br />

<info> app_timer: RTC: initialized.<br />     
<info> app: BLE SIMPLE CENTRAL example started.<br />     
<info> app: esp device found<br />
<info> app: address: A3<br />
<info> app: address: 2C<br />
<info> app: address: CB<br />
<info> app: address: 8B<br />
<info> app: address: 9E<br />
<info> app: address: 75<br />
<info> app: battery percent: 0<br />
<info> app: Connected.<br />
<info> app: serv uuid is 0x42F3<br />
<info> app: event type is  0<br />
<info> app: uuid type is  2<br />
<info> app: uuid type is  2<br />
<info> app: char uuid is 0xFF01<br />
<info> app: char uuid is 0xFF02<br />
<info> app: char uuid is 0xFF03<br />
<info> app: nrf esp service discovered on conn_handle 0x0.<br />
<info> app: serv uuid is 0x1E08<br />
<info> app: event type is  3<br />
<info> app: uuid type is  0<br />
<info> app: uuid type is  2<br />
<info> app: Accelerometer X value is 80.000000<br />
<info> app: Accelerometer Y value is 144.0000004<br />
<info> app: Accelerometer Z value is 32.000000<br />
<info> app: Accelerometer X value is 224.000000<br />
<info> app: Accelerometer Y value is 128.0000004<br />
<info> app: Accelerometer Z value is 128.000000<br />
<info> app: Accelerometer X value is 64.000000<br />
<info> app: Accelerometer Y value is 240.0000004<br />
<info> app: Accelerometer Z value is 64.000000<br />
<info> app: Accelerometer X value is 144.000000<br />
<info> app: Accelerometer Y value is 112.0000004<br />
<info> app: Accelerometer Z value is 32.000<br />
<info> app: Accelerometer X value is 144.000000<br />
<info> app: Accelerometer Y value is 128.0000004<br />
<info> app: Accelerometer Z value is 48.000000<br />
<info> app: Accelerometer X value is 208.000000<br />
<info> app: Accelerometer Y value is 16.000000<br />
<info> app: Accelerometer Z value is 160.000000<br />
<info> app: Accelerometer X value is 128.000000<br />
<info> app: Accelerometer Y value is 144.0000004<br />
<info> app: Accelerometer Z value is 224.000000<br />
<info> app: Accelerometer X value is 240.000000<br />
<info> app: Accelerometer Y value is 192.0000004<br />
<info> app: Accelerometer Z value is 240.000000<br />
<info> app: Accelerometer X value is 192.000000<br />
<info> app: Accelerometer Y value is 192.0000004<br />
<info> app: Accelerometer Z value is 96.000000<br />
<info> app: Accelerometer X value is 112.000000<br />
<info> app: Accelerometer Y value is 32.000000<br />
<info> app: Accelerometer Z value is 224.000000<br />
<info> app: Accelerometer X value is 144.000000<br />
<info> app: Accelerometer Y value is 16.000000<br />
<info> app: Accelerometer Z value is 192.000000<br />
<info> app: Accelerometer X value is 240.000000<br />
<info> app: Accelerometer Y value is 112.0000004<br />
<info> app: Accelerometer Z value is 192.000000<br />
<info> app: Accelerometer X value is 112.000000<br />
<info> app: Accelerometer Y value is 80.000000<br />
<info> app: Accelerometer Z value is 64.000000<br />
<info> app: Accelerometer X value is 224.000000<br />
<info> app: Accelerometer Y value is 48.000000<br />
<info> app: Accelerometer Z value is 160.000000<br />
<info> app: Accelerometer X value is 192.000000<br />
<info> app: Accelerometer Y value is 176.0000004<br />
<info> app: Accelerometer Z value is 80.000000<br />
<info> app: Accelerometer X value is 96.00<br />
<info> app: Accelerometer Y value is 144.0000004<br />
<info> app: Accelerometer Z value is 48.000000<br />
<info> app: Accelerometer X value is 208.000000<br />
<info> app: Accelerometer Y value is 64.000000<br />
<info> app: Accelerometer Z value is 64.000000<br />
<info> app: Accelerometer X value is 144.000000<br />
<info> app: Accelerometer Y value is 128.0000004<br />
<info> app: Accelerometer Z value is 16.000000<br />
<info> app: Accelerometer X value is 96.000000<br />
<info> app: Accelerometer Y value is 16.000000<br />
<info> app: Accelerometer Z value is 96.000000<br />
<info> app: Accelerometer X value is 48.000000<br />
<info> app: Accelerometer Y value is 1<br />
<info> app: Accelerometer Z value is 192.000000<br />
<info> app: Accelerometer X value is 192.000000<br />
<info> app: Accelerometer Y value is 64.000000<br />
<info> app: Accelerometer Z value is 224.000000<br />
<info> app: Accelerometer X value is 16.000000<br />
<info> app: Accelerometer Y value is 160.0000004<br />
<info> app: Accelerometer Z value is 48.000000<br />
<info> app: Accelerometer X value is 160.000000<br />
<info> app: Accelerometer Y value is 160.0000004<br />
<info> app: Accelerometer Z value is 96.000000<br />
<info> app: Accelerometer X value is 48.000000<br />
<info> app: Accelerometer Y value is 48.000000<br />
<info> app: Accelerometer Z value is 112.000000<br />
<info> app: Accelerometer X value is 32.000000<br />
<info> app: Accelerometer Y value is 48.000000<br />
<info> app: Accelerometer Z value is 208.000000<br />
<info> app: Accelerometer X value is 240.000000<br />
<info> app: Accelerometer Y value is 128.0000004<br />
<info> app: Accelerometer Z value is 112.000000<br />
<info> app: Accelerometer X value is 48.000000<br />
<info> app: Accelerometer Y value is 48.000000<br />
<info> app: Accelerometer Z value is 48.000000<br />
<info> app: Accelerometer X value is 192.000000<br />
<info> app: Accelerometer Y value is 3<br />
<info> app: Accelerometer Z value is 32.000000<br />
<info> app: Accelerometer X value is 96.000000<br />
<info> app: Accelerometer Y value is 96.000000<br />
<info> app: Accelerometer Z value is 16.000000<br />
<info> app: Accelerometer X value is 112.000000<br />
<info> app: Accelerometer Y value is 112.0000004<br />
<info> app: Accelerometer Z value is 208.000000<br />
<info> app: Accelerometer X value is 128.000000<br />
<info> app: Accelerometer Y value is 208.0000004<br />
<info> app: Accelerometer Z value is 128.000000<br />
<info> app: Accelerometer X value is 32.000000<br />
<info> app: Accelerometer Y value is 160<br />
<info> app: Accelerometer Z value is 144.000000<br />
<info> app: Accelerometer X value is 192.000000<br />
<info> app: Accelerometer Y value is 64.000000<br />
<info> app: Accelerometer Z value is 176.00000<br />
