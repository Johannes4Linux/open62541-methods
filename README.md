# General

This repository contains an example how to use methods in an open62541 OPC UA Server.

# Content
- myServer.c: A simple OPC UA Server offering two void/void Methods and one method with two input and one output parameters
- myClient.c: A simple OPC UA Client, connecting to the server and calling the three methods
- Makefile: to build the examples

# Dependencies

To compile the code, you must have the open62541 library installed. You can download it here in the download section:

[open62541](https://open62541.org/)

To make the example a little bit more interesting, I used a Raspberry Pi and two GPIOs with the wiringPi library. You can install it with the following command on Raspbian:

````
sudo apt install wiringpi
````

If you want to use the code on a normal PC, just comment the $FLAG variable in the Makefile and you will get prints instead.

I have also connected two LEDs to GPIO 19 and 26 of my Raspberry Pi 3 model B.

# More information

If you want to get more information about this examples, check out my videos on this topics:

- [void/void Method](https://www.youtube.com/watch?v=9piREtLmXtY)
- [Method with input and output parameters](https://www.youtube.com/watch?v=ThMGIfwjtco)
- [Client to call Methods](https://www.youtube.com/watch?v=ziEQ8M9Oh6Q)
