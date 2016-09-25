#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include "RF24.h"
#include "printf.h"

//LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);  // set the LCD address to 0x38

/* Radio config */
//Set this radio as radio number 0 or 1 
bool radioNumber = 0;
// Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 
RF24 radio(7,8);
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
const uint64_t pipe =  0xC2C2C2C2C2LL;

void setup()
{
  //Setup LCD
  //lcd.begin (16,2); 
  //lcd.clear();
  //lcd.print("Hello!");

  //Setup serial port
  Serial.begin(115200);
  printf_begin();
  Serial.println(F("door_monitor_alarm starting..."));
  //Setup radio  
  radio.begin();
  radio.setRetries(15,15);
  radio.setPayloadSize(8);
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setAutoAck(true);
  radio.setChannel(0x60);
  radio.enableDynamicPayloads();
  radio.enableAckPayload();

  radio.openReadingPipe(0, pipe);
  radio.openReadingPipe(1, 0xe7e7e7e7e7LL);

  radio.openWritingPipe(pipe);
  radio.powerUp();   // make sure its up & running in time for first pkt
  radio.printDetails();
}

void sendRadioByte(byte b)
{
  if(!radio.write( &b, sizeof(byte) )){
    Serial.println(F("Write failed"));
  }
}

void loop()
{
  delay(10);
  sendRadioByte(1);
}
