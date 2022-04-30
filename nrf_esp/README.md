-> soft device version is 7.2.0
-> To build project go to "nrf_esp/pca10059/s140/armgcc" and run make all command.
-> hex file of the project will be generated in "nrf_esp/pca10059/s140/armgcc/_build/" folder.
-> Flash Both .hex named s140_nrf52_7.2.0_softdevice.hex and nrf52840_xxaa.hex
Hardware Required:

NRF52840 Dongle 
USB to TTL Board

To Print NRF_LOG:

-> In sdk_config.h enable uart
	#ifndef NRF_LOG_BACKEND_UART_ENABLED
	#define NRF_LOG_BACKEND_UART_ENABLED 1
	#endif
-> In sdk_config.h add uart TX pin in my case it is pin 0.13
	#ifndef NRF_LOG_BACKEND_UART_TX_PIN
	#define NRF_LOG_BACKEND_UART_TX_PIN 13
	#endif
-> connect TX pin and Groud(GND) pin of nrf52840 to the usb to ttl board	

Example Output:

<info> app_timer: RTC: initialized.
<info> app: BLE SIMPLE CENTRAL example started.
<info> app: esp device found
<info> app: address: A3
<info> app: address: 2C
<info> app: address: CB
<info> app: address: 8B
<info> app: address: 9E
<info> app: address: 75
<info> app: battery percent: 0
<info> app: Connected.
<info> app: serv uuid is 0x42F3
<info> app: event type is  0
<info> app: uuid type is  2
<info> app: uuid type is  2
<info> app: char uuid is 0xFF01
<info> app: char uuid is 0xFF02
<info> app: char uuid is 0xFF03
<info> app: nrf esp service discovered on conn_handle 0x0.
<info> app: serv uuid is 0x1E08
<info> app: event type is  3
<info> app: uuid type is  0
<info> app: uuid type is  2
<info> app: Accelerometer X value is 80.000000
<info> app: Accelerometer Y value is 144.0000004
<info> app: Accelerometer Z value is 32.000000
<info> app: Accelerometer X value is 224.000000
<info> app: Accelerometer Y value is 128.0000004
<info> app: Accelerometer Z value is 128.000000
<info> app: Accelerometer X value is 64.000000
<info> app: Accelerometer Y value is 240.0000004
<info> app: Accelerometer Z value is 64.000000
<info> app: Accelerometer X value is 144.000000
<info> app: Accelerometer Y value is 112.0000004
<info> app: Accelerometer Z value is 32.000
<info> app: Accelerometer X value is 144.000000
<info> app: Accelerometer Y value is 128.0000004
<info> app: Accelerometer Z value is 48.000000
<info> app: Accelerometer X value is 208.000000
<info> app: Accelerometer Y value is 16.000000
<info> app: Accelerometer Z value is 160.000000
<info> app: Accelerometer X value is 128.000000
<info> app: Accelerometer Y value is 144.0000004
<info> app: Accelerometer Z value is 224.000000
<info> app: Accelerometer X value is 240.000000
<info> app: Accelerometer Y value is 192.0000004
<info> app: Accelerometer Z value is 240.000000
<info> app: Accelerometer X value is 192.000000
<info> app: Accelerometer Y value is 192.0000004
<info> app: Accelerometer Z value is 96.000000
<info> app: Accelerometer X value is 112.000000
<info> app: Accelerometer Y value is 32.000000
<info> app: Accelerometer Z value is 224.000000
<info> app: Accelerometer X value is 144.000000
<info> app: Accelerometer Y value is 16.000000
<info> app: Accelerometer Z value is 192.000000
<info> app: Accelerometer X value is 240.000000
<info> app: Accelerometer Y value is 112.0000004
<info> app: Accelerometer Z value is 192.000000
<info> app: Accelerometer X value is 112.000000
<info> app: Accelerometer Y value is 80.000000
<info> app: Accelerometer Z value is 64.000000
<info> app: Accelerometer X value is 224.000000
<info> app: Accelerometer Y value is 48.000000
<info> app: Accelerometer Z value is 160.000000
<info> app: Accelerometer X value is 192.000000
<info> app: Accelerometer Y value is 176.0000004
<info> app: Accelerometer Z value is 80.000000
<info> app: Accelerometer X value is 96.00
<info> app: Accelerometer Y value is 144.0000004
<info> app: Accelerometer Z value is 48.000000
<info> app: Accelerometer X value is 208.000000
<info> app: Accelerometer Y value is 64.000000
<info> app: Accelerometer Z value is 64.000000
<info> app: Accelerometer X value is 144.000000
<info> app: Accelerometer Y value is 128.0000004
<info> app: Accelerometer Z value is 16.000000
<info> app: Accelerometer X value is 96.000000
<info> app: Accelerometer Y value is 16.000000
<info> app: Accelerometer Z value is 96.000000
<info> app: Accelerometer X value is 48.000000
<info> app: Accelerometer Y value is 1
<info> app: Accelerometer Z value is 192.000000
<info> app: Accelerometer X value is 192.000000
<info> app: Accelerometer Y value is 64.000000
<info> app: Accelerometer Z value is 224.000000
<info> app: Accelerometer X value is 16.000000
<info> app: Accelerometer Y value is 160.0000004
<info> app: Accelerometer Z value is 48.000000
<info> app: Accelerometer X value is 160.000000
<info> app: Accelerometer Y value is 160.0000004
<info> app: Accelerometer Z value is 96.000000
<info> app: Accelerometer X value is 48.000000
<info> app: Accelerometer Y value is 48.
<info> app: Accelerometer Z value is 112.000000
<info> app: Accelerometer X value is 32.000000
<info> app: Accelerometer Y value is 48.000000
<info> app: Accelerometer Z value is 208.000000
<info> app: Accelerometer X value is 240.000000
<info> app: Accelerometer Y value is 128.0000004
<info> app: Accelerometer Z value is 112.000000
<info> app: Accelerometer X value is 48.000000
<info> app: Accelerometer Y value is 48.000000
<info> app: Accelerometer Z value is 48.000000
<info> app: Accelerometer X value is 192.000000
<info> app: Accelerometer Y value is 3
<info> app: Accelerometer Z value is 32.000000
<info> app: Accelerometer X value is 96.000000
<info> app: Accelerometer Y value is 96.000000
<info> app: Accelerometer Z value is 16.000000
<info> app: Accelerometer X value is 112.000000
<info> app: Accelerometer Y value is 112.0000004
<info> app: Accelerometer Z value is 208.000000
<info> app: Accelerometer X value is 128.000000
<info> app: Accelerometer Y value is 208.0000004
<info> app: Accelerometer Z value is 128.000000
<info> app: Accelerometer X value is 32.000000
<info> app: Accelerometer Y value is 160
<info> app: Accelerometer Z value is 144.000000
<info> app: Accelerometer X value is 192.000000
<info> app: Accelerometer Y value is 64.000000
<info> app: Accelerometer Z value is 176.00000