# haltech_keypad_emulator
Emulator to simulate a Haltech Canbus Keypad for input expansion.

This sketch is deisgned to run on any arduino compatible board with an MCP2515 controller and can tranceiver.

### The following functions are performed:
* Negotiate with Haltech ECU as 3x5 Haltech keypad and manage keepalives
* Output to keypad with status based on other inputs.

## Dependencies:
* https://github.com/coryjfowler/MCP_CAN_lib
* https://github.com/RobTillaart/PCF8574
