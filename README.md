# Reading-Temperature

This is a simple project written in C and Python to read temperatures from many places and to collect data in database. 

Project consists of:
- part for Arduino (in C language)
- part for Raspberry Pi. 

On Aduino there is reading temperature (sensors DS18B20). Then data are sending to Raspberry Pi.

Raspberry Pi is a server. There is database and small program to display and analize data. 

Communication between Arduino and Raspberry Pi is implemented using radio module NRF24L01.
