#include <RF24.h>
#include <SPI.h>

//ce, csn pins
RF24 radio(9,10); //ce,csn

void setup(void) {
  Serial.begin(9600);
 radio.begin();
 radio.setPALevel(RF24_PA_MAX);
 radio.setChannel(0x4C);
 //radio.setDataRate(RF24_250KBPS);
 //radio.setDataRate(RF24.BR_1MBPS);
 //radio.setCRCLength(RF24_CRC_16);
 //radio.setAutoAck();
 //radio.setPayloadSize();
 radio.openWritingPipe(0xF0F0F0F0E1LL);
 //const byte tab[6]= "00009";
 //radio.openWritingPipe(tab);
 radio.enableDynamicPayloads();
 radio.powerUp();
 Serial.println("witaj");
}

void loop() {
 const char text[] = "Hello World!";
 radio.write(&text,sizeof(text));
 Serial.println(text);
 delay(1000);
}



