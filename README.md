# lmic-rpi-lora-gps-hat

# 2017-04-12 WORK UNDER CONSTRUCTION 

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

Modifications necessary: None

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

## Example "join"
Directory: /examples/join

Modifications necessary: 

File /examples/join/main.c:

* Adapt "application router ID (LSBF)" according for your network infrastructure.
  In case of The Things Network, this is the "Application EUI" of the application
  created in the TTN console. Double check that you use the LSB (least significant
  byte first) notation of the Application EUI.

* Adapt "unique device ID (LSBF)" according for your network infrastructure.
  In case of The Things Network, you need to register a new device using TTN console.
  Copy this "Device EUI" from the console and make sure you use the LSB notation.

* Adapt "device-specific AES key (derived from device EUI)". 
  This is the secret shared between your device and The Things Network. In TTN
  terms this is known as "(LoRa) App Key".
  Copy this 16 bytes and stick to MSB notation (Most significant byte first)

File /lmic/lmic.c:

The LMIC 1.6 stack randomly chooses one of six frequencies to send the "join" message
to the network. Tesing with a Kerlink IoT Station, only the following frequencies
worked: 868.1 868.3 868.5 MHz
The following default frequency did not work: 864.1 864.3 864.5
For this reason, I modified the code to only randomly choose between the three
working join frequencies.


This example verifies that the radio is working and that the node settings are 
correct and match your network infrastructure. It uses OTAA (Over the Air Activiation)
to register the node. Note that this example _won't_
work with a Single Channel Gateway.

    cd examples/join
    make clean
    make
    sudo ./build/join.out

Possible outpout:

    000000000 HAL: Initializing ...
    000000001 HAL: Set radio RST pin to 0x00
    000000002 HAL: Wait until 000000002 ms
    000000003 HAL: Set radio RST pin to 0x02
    000000003 HAL: Wait until 000000008 ms
    000000008 HAL: Receiving ...
    000000020 Debug: Initializing
    000000020 Debug: JOINING
    000002625 Debug: EV_TXSTART
    000002626 HAL: Sending ...
    000007689 HAL: Receiving ...
    000007689 HAL: Wait until 000007690 ms
    000007762 Debug: JOINED

What can be seen is that after sending the "join" message, the LMIC stack waits
5 seconds for the receive window and receives the acknowledgement from the LoRa gateway.

