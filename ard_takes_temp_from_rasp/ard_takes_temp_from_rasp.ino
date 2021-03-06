#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <dht.h>
 
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
 
//DHTSENSOR
dht DHT;
#define DHT11PINDATA 3
#define DHT11PINVCC  2
float temperature;
float humidity;
 
// NRF24L01
// Set up nRF24L01 radio on SPI pin for CE, CSN
RF24 radio(7,8); //9,10
// Example below using pipe5 for writing
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0x7365727631LL };
 
char receivePayload[32];
uint8_t counter=0;
 
//count number of entering into main loop
uint8_t loopCounter=109;  
 
//WATCHDOG
volatile int f_wdt=1;
 
ISR(WDT_vect) {
  if(f_wdt == 0) {
    f_wdt=1;
  } else {
    //Serial.println("WDT Overrun!!!");
  }
}
 
void enterSleep(void) {
  radio.powerDown();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
  sleep_enable();
 
  /* Now enter sleep mode. */
  sleep_mode();
 
  /* The program will continue from here after the WDT timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
 
  /* Re-enable the peripherals. */
  power_all_enable();
}
 
 
void setup() {
 
  Serial.begin(9600);
 
  //WATCHDOG
  /* Clear the reset flag. */
  MCUSR &= ~(1<<WDRF);
  /* In order to change WDE or the prescaler, we need to
   * set WDCE (This will allow updates for 4 clock cycles).
   */
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  /* set new watchdog timeout prescaler value */
  WDTCSR = 1<<WDP0 | 1<<WDP3; /* 8.0 seconds */
  /* Enable the WD interrupt (note no reset). */
  WDTCSR |= _BV(WDIE);
 
  //CONFIGURE DHT11
  pinMode(DHT11PINVCC, OUTPUT);
  //make it blink at startup
  digitalWrite(DHT11PINVCC, HIGH);
  delay(500);
  digitalWrite(DHT11PINVCC, LOW);
 
  //CONFIGURE RADIO
  radio.begin();
  // Enable this seems to work better
  radio.enableDynamicPayloads();
  radio.setAutoAck(1);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(70);
  radio.setRetries(15,15);
  radio.setCRCLength(RF24_CRC_8);
 
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
 
}
 
void loop() {
  if(f_wdt == 1) {
    loopCounter ++;
    if (loopCounter > 6) {
      // 110 *8 = 880s = 14.6min @ 16mhtz
      readSensor();
      sendOverRadio();
      loopCounter = 0;
    }
    f_wdt = 0;
    enterSleep();
  } else {
    //nothing
  }
}
 
void sendOverRadio() {
  radio.powerUp();
  //prepare 
  uint8_t data1 = 0;
  char temp[5];
  bool timeout=0;
  uint16_t nodeID = pipes[0] & 0xff;
  char buffer[12];
  char outBuffer[32]="";
  unsigned long send_time, rtt = 0;
 
  //data1 is a counter
  data1 = counter++;
  if ( counter > 999 ) counter = 0;
 
  // Append the hex nodeID to the beginning of the payload
  sprintf(outBuffer,"%2X",nodeID);
  strcat(outBuffer,",");
  sprintf(temp,"%03d",data1);
  strcat(outBuffer,temp);
  strcat(outBuffer,",");
 
  //read sensor
  strcat(outBuffer,dtostrf(temperature, 4,2, buffer));
  strcat(outBuffer,",");
  strcat(outBuffer,dtostrf(humidity, 4,2, buffer));
 
  // Stop listening and write to radio
  radio.stopListening();
 
  // Send to hub
  if ( radio.write( outBuffer, strlen(outBuffer)) ) {
    printf("Send successful\n\r");
  } else {
    printf("Send failed\n\r");
  }
 
  //wait response
  radio.startListening();
  delay(20);
  while ( radio.available() && !timeout ) {
      uint8_t len = radio.getDynamicPayloadSize();
      radio.read( receivePayload, len);
 
      receivePayload[len] = 0;
      // Compare receive payload with outBuffer
      if ( ! strcmp(outBuffer, receivePayload) ) {
          rtt = millis() - send_time;
          //Serial.println("inBuffer --> rtt:");
          //Serial.println(rtt);
      }
 
      // Check for timeout and exit the while loop
      if ( millis() - send_time > radio.getMaxTimeout() ) {
          //Serial.println("Timeout!!!");
          timeout = 1;
      }
 
      delay(10);
  } // End while
}
 
void readSensor() {
  digitalWrite(DHT11PINVCC, HIGH);
  delay(200);
  int chk = DHT.read11(DHT11PINDATA);
  //Serial.print("Read sensor: ");
  switch (chk) {
    case DHTLIB_OK: 
      //Serial.println("OK"); 
      break;
    case DHTLIB_ERROR_CHECKSUM: 
      //Serial.println("Checksum error"); 
      break;
    case DHTLIB_ERROR_TIMEOUT: 
      //Serial.println("Time out error"); 
      break;
    default: 
      //Serial.println("Unknown error"); 
      break;
  }
  temperature = DHT.temperature;
  humidity = DHT.humidity;
 
  digitalWrite(DHT11PINVCC, LOW);
}
