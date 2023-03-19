#include <mcp_can.h>
#include <SPI.h>
#include <PCF8574.h>
#include <Wire.h>

PCF8574 pcf20(0x38);


long unsigned int rxId;  // storage for can data
unsigned char len = 0;   // storage for can data
unsigned char rxBuf[8];  // storage for can data

#define CAN0_INT 2  // Set INT to pin 2
MCP_CAN CAN0(10);   // set CS pin to 10r



unsigned long task1Interval = 50;  // 50ms interval for keep aliv frame
unsigned long task2Interval = 50;  // 50ms interval for analogue value sending
unsigned long task3Interval = 10;  // 3 second interval for task 3
unsigned long task4Interval = 40;  // 4 second interval for task 4
unsigned long task1Millis = 0;     // storage for millis counter
unsigned long task2Millis = 0;     // storage for millis counter
unsigned long task3Millis = 0;     // storage for millis counter
unsigned long task4Millis = 0;     // storage for millis counter


bool button1;
bool button2;


void setup() {
  // start serial port an send a message with delay for starting
  Serial.begin(115200);
  Serial.println("analog reading to 12 bit test");
  delay(1000);

  // initialize canbus with 1000kbit and 16mhz xtal
  if (CAN0.begin(MCP_ANY, CAN_1000KBPS, MCP_16MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else Serial.println("Error Initializing MCP2515...");

  // Set operation mode to normal so the MCP2515 sends acks to received data.
  CAN0.setMode(MCP_NORMAL);

  pinMode(CAN0_INT, INPUT);      // set INT pin to be an input
  digitalWrite(CAN0_INT, HIGH);  // set INT pin high to enable interna pullup

  pinMode(5, INPUT);
  digitalWrite(5, HIGH);
  pinMode(6, INPUT);
  digitalWrite(6, HIGH);


  pcf20.begin();
  
  Serial.println("All OK");  // all ready to go !
}




void loop() {

  unsigned long currentMillis = millis();  // Get current time in milliseconds

  // Execute task 1 every 1 second
  if (currentMillis - task1Millis >= task1Interval) {
    task1Millis = currentMillis;
    SendKeepAlive();
  }

  // Execute task 2 every 5 seconds
  if (currentMillis - task2Millis >= task2Interval) {
    task2Millis = currentMillis;
    SendAnalogValues();
  }

  // Execute task 3 every 3 seconds
  if (currentMillis - task3Millis >= task3Interval) {
    task3Millis = currentMillis;
    SendButtonInfo();
  }

  // Execute task 4 every 4 seconds
  if (currentMillis - task4Millis >= task4Interval) {
    task4Millis = currentMillis;
    task4();
  }

  // read can buffer when interrupted and jump to canread for processing.
  if (!digitalRead(CAN0_INT))  // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);  // Read data: len = data length, buf = data byte(s)
                                          // Serial.println("interuppted and read can !");
    canRead();
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
      //      SerialUSB.print("Type 34 frame identified ");
      //      SerialUSB.print("\r\n");

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
      //      SerialUSB.print("Type 66 Frame Identified");
      //      SerialUSB.print("\r\n");

    } else if (((rxBuf[0]) == 0) && ((rxBuf[7]) == 200)) {
      b0 = 128;
      b1 = 0;
      b2 = 0;
      b3 = 0;
      b4 = 1;
      b5 = 0;
      b6 = 4;
      b7 = 5;
      //      SerialUSB.print("Type 0 Frame Identified");
      //      SerialUSB.print("\r\n");
    }
    byte txBuf[8] = { b0, b1, b2, b3, b4, b5, b6, b7 };
    Serial.print(b0);
    Serial.print(b1);
    Serial.print(b2);
    Serial.print(b3);
    Serial.print(b4);
    Serial.print(b5);
    Serial.print(b6);
    Serial.println(b7);
    CAN0.sendMsgBuf(1421, 0, 8, txBuf);
  }
}

void SendButtonInfo() {
  byte ButtonInfo[3];
  bitWrite(ButtonInfo[0], 0, !pcf20.readButton(0));
  bitWrite(ButtonInfo[0], 1, !pcf20.readButton(1));
  bitWrite(ButtonInfo[0], 2, 0);
  bitWrite(ButtonInfo[0], 3, 0);
  bitWrite(ButtonInfo[0], 4, 0);
  bitWrite(ButtonInfo[0], 5, 0);
  bitWrite(ButtonInfo[0], 6, 0);
  bitWrite(ButtonInfo[0], 7, 0);

  
  ButtonInfo[1] = 0;
  ButtonInfo[2] = 0;
  CAN0.sendMsgBuf(0x18D, 0, 3, ButtonInfo);
}












void task4() {}



void SendKeepAlive() {
  byte KeepAlive[1] = { 5 };
  CAN0.sendMsgBuf(0x70D, 0, 5, KeepAlive);
}

void SendAnalogValues() {
  //CAN0.sendMsgBuf(0x2C1, 0, 8, SendAnalogue);  // send the can message onto the bus
}
