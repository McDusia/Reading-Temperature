    /*
    * Arduino Wireless Communication Tutorial
    *     Example 1 - Transmitter Code
    *
    * by Dejan Nedelkovski, www.HowToMechatronics.com
    *
    * Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
    */
    #include <SPI.h>
    #include <nRF24L01.h>
    #include <RF24.h>
    #include <printf.h>
    #include <OneWire.h>
    #include <DallasTemperature.h>


 #define ONE_WIRE_BUS 4


// Setup a oneWire instance to communicate with ANY OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Set oneWire reference to Dallas Temperature sensor.
DallasTemperature sensors(&oneWire);


    RF24 radio(7, 8); // CE, CSN
    const byte address[6] = "00009";
    void setup() {
      Serial.begin(9600);
      printf_begin(); // <------- to do info o nadajniku
      sensors.begin();
      radio.begin();
      radio.openWritingPipe(address);
      radio.setPALevel(RF24_PA_MIN);
      radio.stopListening();
     radio.printDetails(); // <------- to do info o nadajniku
      Serial.println("initialized");
    }
    void loop() {
      sensors.requestTemperatures();
      double temp = sensors.getTempCByIndex(0);
      Serial.println(temp);

      // print do radia


      const char text[] = "Hello World";
      bool s = radio.write(&temp, sizeof(temp));
      delay(1000);
      Serial.println(s);
      //radio.printDetails();
    }
