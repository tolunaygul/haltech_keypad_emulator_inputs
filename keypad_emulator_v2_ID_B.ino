#include <mcp_can.h>  // library for MCP2515 ic
#include <SPI.h>      // library for SPI communication
#include <PCF8574.h>  // library for PCF8574 IO expander
#include <Wire.h>     // library for I2C communication

PCF8574 pcf20(0x38);  // initialize PCF8574 IC at adress 0X38 ( A0 A1 A2 to ground)


long unsigned int rxId;  // storage for can data
unsigned char len = 0;   // storage for can data
unsigned char rxBuf[8];  // storage for can data

#define CAN0_INT 2  // Set INT to pin 2
MCP_CAN CAN0(10);   // set CS pin to 10r



unsigned long KAinterval = 150;              // 50ms interval for keep aliv frame
unsigned long ButtonInfoInterval = 30;      // 30ms interval for button info frame
unsigned long KAintervalMillis = 0;         // storage for millis counter
unsigned long ButtonInfoIntervalMillis = 0; // storage for millis counter



void setup() {
  // start serial port an send a message with delay for starting
  Serial.begin(115200);
  Serial.println("Haltech 3x5 keypad ID B emulator");
  delay(1000);

  // initialize canbus with 1000kbit and 16mhz xtal
  if (CAN0.begin(MCP_ANY, CAN_1000KBPS, MCP_16MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else Serial.println("Error Initializing MCP2515...");

  // Set operation mode to normal so the MCP2515 sends acks to received data.
  CAN0.setMode(MCP_NORMAL);

  pinMode(CAN0_INT, INPUT);      // set INT pin to be an input
  digitalWrite(CAN0_INT, HIGH);  // set INT pin high to enable interna pullup

  pcf20.begin();  // start the PCF library
  
  Serial.println("All OK");  // all ready to go !
}




void loop() {

  unsigned long currentMillis = millis();  // Get current time in milliseconds

  // Execute keepalive frame every 50 ms
  if (currentMillis - KAintervalMillis >= KAinterval) {
    KAintervalMillis = currentMillis;
    SendKeepAlive();
  }

  // Execute buttoninfo frame every 30 ms
  if (currentMillis - ButtonInfoIntervalMillis >= ButtonInfoInterval) {
    ButtonInfoIntervalMillis = currentMillis;
    SendButtonInfo();
  }

  // read can buffer when interrupted and jump to canread for processing.
  if (!digitalRead(CAN0_INT))  // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);  // Read data: len = data length, buf = data byte(s)
    canRead(); // execute canRead function to negotiate with ecu
  }
}


void canRead() {
  // CAN Input from Haltech Keypad
  int b0;
  int b1;
  int b2;
  int b3;
  int b4;
  int b5;
  int b6;
  int b7;

  // Keypad Configuration Section

  if (rxId == 0X60D) {
    if ((rxBuf[0]) == 34) {
      b0 = 96;
      b1 = (rxBuf[1]);
      b2 = (rxBuf[2]);
      b3 = (rxBuf[3]);
      b4 = 0;
      b5 = 0;
      b6 = 0;
      b7 = 0;

    } else if ((rxBuf[0]) == 66) {
      b0 = 67;
      b1 = (rxBuf[1]);
      b2 = (rxBuf[2]);
      b3 = (rxBuf[3]);
      if ((b1 == 24) && (b2 == 16) && (b3 == 1)) {
        b4 = 7;
        b5 = 4;
        b6 = 0;
        b7 = 0;
      } else if ((b1 == 24) && (b2 == 16) && (b3 == 2)) {
        b4 = 75;
        b5 = 51;
        b6 = 0;
        b7 = 0;
      } else if ((b1 == 24) && (b2 == 16) && (b3 == 3)) {
        b4 = 1;
        b5 = 0;
        b6 = 0;
        b7 = 0;
      } else if ((b1 == 24) && (b2 == 16) && (b3 == 4)) {
        b4 = 166;
        b5 = 184;
        b6 = 25;
        b7 = 12;
      } else if ((b1 == 0) && (b2 == 24) && (b3 == 1)) {
        b4 = 141;
        b5 = 1;
        b6 = 0;
        b7 = 64;
      } else {
        b4 = 0;
        b5 = 0;
        b6 = 0;
        b7 = 0;
      }

    } else if (((rxBuf[0]) == 0) && ((rxBuf[7]) == 200)) {
      b0 = 128;
      b1 = 0;
      b2 = 0;
      b3 = 0;
      b4 = 1;
      b5 = 0;
      b6 = 4;
      b7 = 5;

    }
    byte txBuf[8] = { b0, b1, b2, b3, b4, b5, b6, b7 };
    CAN0.sendMsgBuf(0x58D, 0, 8, txBuf);
  }
}

void SendButtonInfo() {
  byte ButtonInfo[3];                                 // declare an array for 3 bytes used for key pressed information
  bitWrite(ButtonInfo[0], 0, 0);                      // byte 0, bit 0, button 1 
  bitWrite(ButtonInfo[0], 1, 0);                      // byte 0, bit 0, button 2
  bitWrite(ButtonInfo[0], 2, 0);                      // byte 0, bit 0, button 3
  bitWrite(ButtonInfo[0], 3, 0);                      // byte 0, bit 0, button 4
  bitWrite(ButtonInfo[0], 4, 0);                      // byte 0, bit 0, button 5
  bitWrite(ButtonInfo[0], 5, 0);                      // byte 0, bit 0, button 6
  bitWrite(ButtonInfo[0], 6, 0);                      // byte 0, bit 0, button 7
  bitWrite(ButtonInfo[0], 7, 0);                      // byte 0, bit 0, button 8

  bitWrite(ButtonInfo[1], 0, 0);                      // byte 1, bit 0, button 9
  bitWrite(ButtonInfo[1], 1, 0);                      // byte 1, bit 1, button 10
  bitWrite(ButtonInfo[1], 2, 0);                      // byte 1, bit 2, button 11
  bitWrite(ButtonInfo[1], 3, 0);                      // byte 1, bit 2, button 12
  bitWrite(ButtonInfo[1], 4, 0);                      // byte 1, bit 3, button 13
  bitWrite(ButtonInfo[1], 5, 0);                      // byte 1, bit 4, button 14
  bitWrite(ButtonInfo[1], 6, 0);                      // byte 1, bit 5, button 15
  bitWrite(ButtonInfo[1], 7, 0);

  ButtonInfo[2] = 0;                                  // byte 3 filled with 0
  CAN0.sendMsgBuf(0x18D, 0, 3, ButtonInfo);           // send the 3 byte data buffer at adres 18D
}


void SendKeepAlive() {                                // send keep alive frame
  byte KeepAlive[1] = { 5 };                          // frame dat is 0X05 for byte 0
  CAN0.sendMsgBuf(0x70D, 0, 1, KeepAlive);            // send the frame at 70D 
}