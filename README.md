# lmic-rpi-lora-gps-hat


# 2017-04-12 WORK UNDER CONSTRUCTION - DO NOT CLONE OR DOWNLOAD

Hardware Abstraction Layer (HAL) for IBM's LMIC 1.6 communication stack 
targeted to RPi and Dragino LoRA/GPS HAT.

The goal was to keep the LMIC 1.6 sourcecode untouched, and just provide a
Hardware Abstraction Layer (HAL) for Raspberry Pi and Dragino LoRa/GPS HAT.

## Installation

### WiringPi
To control the RPi's GPI ports, the WiringPi GPIO interface library has to
be installed. On some operating systems WiringPi is already installed per
default. For instructions on manual installation please refer to the 
following site: 
http://wiringpi.com/download-and-install/

### Enable SPI interface
Per default, the SPI ports on the Raspberry Pi are disabled. You need to
manually enable them using raspi-config.
Follow the instructions given here: 
https://www.raspberrypi.org/documentation/hardware/raspberrypi/spi/README.md

## Note on LMIC 1.6 license
Text copied from https://www.research.ibm.com/labs/zurich/ics/lrsc/lmic.html

IBM "LoRa WAN in C" is the LoRa WAN implementation of choice, and a perfect
match to the IBM LRSC on the end device. It is provided as open source under 
the BSD License.

## Example "hello"
Directory: /examples/hello

Modifications neccessary: None

This example does not use radio, it just periodically logs a counter value.
Can be used to checked if the timer implementation on RPi works as expected.

    cd examples/hello
    make clean
    make
    sudo ./build/hello.out

Possible output:

    000000000 HAL: Initializing ...
    000000003 HAL: Set radio RST pin to 0x00
    000000003 HAL: Wait until 000000004 ms
    000000005 HAL: Set radio RST pin to 0x02
    000000005 HAL: Wait until 000000010 ms
    000000011 HAL: Receiving ...
    000000034 Debug: Initializing
    000000034 Debug: Hello World!
    
    000000034 Debug: Label 'cnt = ' value 0x0
    000001034 Debug: Hello World!
    
    000001034 Debug: Label 'cnt = ' value 0x1
    000002034 Debug: Hello World!
    
    000002034 Debug: Label 'cnt = ' value 0x2
    000003034 Debug: Hello World!
    
    000003034 Debug: Label 'cnt = ' value 0x3

