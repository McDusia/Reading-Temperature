#include <RF24.h>
#include <SPI.h>
#include <OneWire.h>
#include <DS18B20.h>

// Numer pinu do którego podłacza sie czujnik
#define ONEWIRE_PIN 2

// Adres czujnika
byte address[8];
int myNumber;

OneWire onewire(ONEWIRE_PIN);

DS18B20 sensors(&onewire);

//ce, csn pins
RF24 radio(9,10);

void setup(void) {
  Serial.begin(9600);
 radio.begin();
 radio.setPALevel(RF24_PA_MAX);
 radio.setChannel(0x4C);
 radio.openWritingPipe(0xF0F0F0F0E1LL);
 radio.enableDynamicPayloads();
 radio.powerUp();

 sensors.begin();
 sensors.request(address);
}

void loop() {

  onewire.reset_search();
  while(onewire.search(address))
  {
    if (address[0] != 0x28)
      continue;

    if (OneWire::crc8(address, 7) != address[7])
    {
      Serial.println(F("Błędny adres, sprawdz polaczenia"));
      break;
    }

  }
  delay(1000);
  
  if (sensors.available())
  {
    float temperature = sensors.readTemperature(address);
    char temp[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    dtostrf(temperature, 4, 3, temp);
    
    Serial.println(temperature);
    Serial.println(temp);
    Serial.println(F(" 'C"));

  char tmp[8];
    for(int i = 0; i < 8; i++) 
      sprintf(tmp, "%02X", (unsigned char)address[i]);
    sensors.request(address);
    temp[5] = ' ';
   
    int i = 6;
    for(int j = 0; j<8;j++){
      temp[i]= tmp[j];
      i++;
    }
      
   Serial.println(temp);
   radio.write(&temp,sizeof(temp));
    
    delay(4000);
  }
 
}



