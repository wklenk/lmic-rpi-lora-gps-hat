# lmic-rpi-lora-gps-hat

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
Can be used to checked if the timer implementation on RPi works as expected
and if SPI communication with the radio board is possible.

    cd examples/hello
    make clean
    make
    sudo ./build/hello.out

Possible output:

    000000000 HAL: Initializing ...
    000000003 HAL: Detected SX1276 radio module.
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
    000000000 HAL: Detected SX1276 radio module.
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

## Example "periodic"
Directory: /examples/periodic

Modifications necessary: 

File /examples/periodic/main.c:

* Adapt "application router ID (LSBF)" like already described under examples/join.
* Adapt "unique device ID (LSBF)" like already described under examples/join.
* Adapt "device-specific AES key " like already described under examples/join.

File /examples/periodic/sensor.c:

Added code that reads the CPU temperature of the RPi and returns it as a 2 byte integer
value.

This examples does a "joins" the network and then sends a sensor value (the CPU temperature)
every 60 seconds as an unconfirmed message with a payload of 2 bytes.

    cd examples/periodic
    make clean
    make
    sudo ./build/periodic.out

Possible output:

    000000000 HAL: Initializing ...
    000000004 HAL: Detected SX1276 radio module.
    000000004 HAL: Set radio RST pin to 0x00
    000000005 HAL: Wait until 000000006 ms
    000000006 HAL: Set radio RST pin to 0x02
    000000006 HAL: Wait until 000000011 ms
    000000013 HAL: Receiving ...
    000000041 Debug: Initializing
    000000041 Debug: JOINING
    000004897 Debug: EV_TXSTART
    000004898 HAL: Sending ...
    000009960 HAL: Receiving ...
    000009961 HAL: Wait until 000009962 ms
    000010033 Debug: JOINED
    000010034 Debug: 54230
    000010034 Debug: Label 'val = ' value 0xd3d6
    000010034 Debug: EV_TXSTART
    000010034 HAL: Sending ...
    000011081 HAL: Receiving ...
    000011081 HAL: Wait until 000011082 ms
    000012128 HAL: Receiving ...
    000012128 HAL: Wait until 000012130 ms
    000016360 Debug: TXCOMPLETE
    000070068 Debug: 53692
    000070068 Debug: Label 'val = ' value 0xd1bc
    000070069 Debug: EV_TXSTART
    000070070 HAL: Sending ...
    000071117 HAL: Receiving ...
    000071117 HAL: Wait until 000071118 ms
    000072164 HAL: Receiving ...
    000072164 HAL: Wait until 000072165 ms
    000076734 Debug: TXCOMPLETE


