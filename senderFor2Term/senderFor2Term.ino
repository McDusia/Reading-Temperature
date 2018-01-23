#include <RF24.h>
#include <SPI.h>
#include <OneWire.h>
#include <DS18B20.h>

// Numer pinów do których podłacza sie czujniki
#define ONEWIRE_PIN 2
#define ONEWIRE_PIN2 3

// Adresy czujników
byte address[8];
byte address2[8];

OneWire onewire(ONEWIRE_PIN);
OneWire onewire2(ONEWIRE_PIN2);

DS18B20 sensors(&onewire);
DS18B20 sensors2(&onewire2);

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
 sensors2.begin();
 sensors2.request(address2);
}

void loop() {
  //odczytanie adresów czujników
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

  onewire2.reset_search();
  while(onewire2.search(address2))
  {
    if (address2[0] != 0x28)
      continue;

    if (OneWire::crc8(address2, 7) != address2[7])
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
    
    
    sensors.request(address);
    //give sensor a unique name
    char tmp[8];
    for(int i = 0; i < 8; i++) 
      sprintf(tmp, "%02X", (unsigned char)address[i]);
    
    temp[5] = ' ';
   
    int i = 6;
    for(int j = 0; j<8;j++){
      temp[i]= tmp[j];
      i++;
    }
   Serial.println(temperature);
    Serial.println(temp);
    Serial.println(F(" 'C"));

   radio.write(&temp,sizeof(temp));
    
    delay(4000);
  }

  if (sensors2.available())
  {
    float temperature = sensors2.readTemperature(address2);
    char temp[15] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    dtostrf(temperature, 4, 3, temp);   

    
    sensors2.request(address2);
    //give sensor a unique name
    char tmp[8];
    for(int i = 0; i < 8; i++) 
      sprintf(tmp, "%02X", (unsigned char)address2[i]);
    
    temp[5] = ' ';
   
    int i = 6;
    for(int j = 0; j<8;j++){
      temp[i]= tmp[j];
      i++;
    }

    Serial.println("second termometer:");
    Serial.println(temperature);
    Serial.println(temp);
    Serial.println(F(" 'C"));

   radio.write(&temp,sizeof(temp));
    
    delay(4000);
  }
 
}



