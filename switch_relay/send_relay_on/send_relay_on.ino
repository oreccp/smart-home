#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <NewPing.h>
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

/* Ultrasonic ranger */
#define  TRIGGER_PIN  6
#define  ECHO_PIN     5
#define MAX_DISTANCE 500 // Maximum distance we want to ping for (in centimeters).
// NewPing setup of pins and maximum distance.
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Globals
int num_counts_above_threshold = 0;

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

void sendRadioByte(byte b, int retries)
{
  while (!radio.write(&b, sizeof(byte)) && retries > 0){
    Serial.println(F("Write failed"));
    retries--;
  }
}

int readSonar() {
  int dist_cm = sonar.ping_cm();
  Serial.print("Ping: ");
  Serial.print(dist_cm); 
  Serial.println(" cm");
  return dist_cm;  
}

void sendToggleCommand() {
  Serial.println("Command sent");
  // Send the toggle light command
  sendRadioByte(1, 10);
}

void tick() {
  int dist_cm = readSonar();
  if (dist_cm > 0) {
    num_counts_above_threshold++;
  } else {
    num_counts_above_threshold = 0;
  }
  if (num_counts_above_threshold > 10) {
    sendToggleCommand();
    num_counts_above_threshold = 0;
    // Make sure we can toggle at most once a second
    delay(1000);
  }
}

void loop() {
  delay(100);
  tick();
}
