.. zephyr:code-sample:: uart
   :name: UART echo
   :relevant-api: uart_interface

   Read data from the console and echo it back.

Overview
********

This sample demonstrates how to use the UART serial driver with a simple
echo bot. It reads data from the console and echoes the characters back after
an end of line (return key) is received.

The polling API is used for sending data and the interrupt-driven API
for receiving, so that in theory the thread could do something else
while waiting for incoming data.

By default, the UART peripheral that is normally used for the Zephyr shell
is used, so that almost every board should be supported.

Building and Running
********************

Build and flash the sample as follows, changing ``nrf52840dk/nrf52840`` for
your board:

.. zephyr-app-commands::
   :zephyr-app: samples/drivers/uart/echo_bot
   :board: nrf52840dk/nrf52840/arduino_uno_r4_minima
   :goals: build flash
   :compact:


**Building the Project**

To build the project for the Arduino Uno R4 Minima board, use the following command:

```bash
west build -p always -b arduino_uno_r4_minima ../rakshak