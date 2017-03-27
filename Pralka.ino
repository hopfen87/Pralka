/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Refactored and addjusted for Air humidifier by Marcin Chmielewski <marcin.chmielewski@gmail.com>
 * Copyright (C) 2013-2015 Sensnology AB & 2017 Hopfen87
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 * The letest varsion of Air Humidifier is available here: https://github.com/hopfen87/Pralka
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * REVISION HISTORY
 *  * Version 1.0 - Marcin Chmielewski
 * 
 * 
 */

#define MY_NODE_ID 4
// Enable debug prints
#define MY_DEBUG

// Enable and select radio type attached
#define MY_RADIO_NRF24
//#define MY_RADIO_RFM69

#include <SPI.h>
#include <MySensors.h>   

#define RELAY_1  3  // Arduino Digital I/O pin number for first relay PIN 3 for POWER BUTTON
#define NUMBER_OF_RELAYS 1 // Total number of attached relays 
#define RELAY_ON 1  // GPIO value to write to simulate not pressed button
#define RELAY_OFF 0 // GPIO value to write to simulate pressed button

#define READY_PIN 4
#define WORKING_PIN 5
#define FINISHED_PIN  6

#define READY_CHILD_ID 2
#define WORKING_CHILD_ID 3
#define FINISHED_CHILD_ID 4


int readyOldValue = -1;
int workingOldValue = -1;
int finishedOldValue = -1;


MyMessage msgReady(READY_CHILD_ID, V_STATUS);
MyMessage msgWorking(WORKING_CHILD_ID, V_STATUS);
MyMessage msgFinished(FINISHED_CHILD_ID, V_STATUS);


void setup()  
{ 

  for (int sensor=1, pin=RELAY_1; sensor<=NUMBER_OF_RELAYS;sensor++, pin++) {
    // Then set relay pins in output mode
    pinMode(pin, OUTPUT);   
    // Set relay (push button) default value
    digitalWrite(pin, RELAY_ON);
  }
    pinMode(READY_PIN, INPUT_PULLUP);
    pinMode(WORKING_PIN, INPUT_PULLUP);
    pinMode(FINISHED_PIN, INPUT_PULLUP);

}

void presentation()  
{ 
  // Send the Sketch Version Information to the Gateway
  sendSketchInfo("Pralka", "1.0");

  for (int sensor=1, pin=RELAY_1; sensor<=NUMBER_OF_RELAYS;sensor++, pin++) {
    // Register all sensors to gw (they will be created as child devices)
    present(sensor, S_BINARY);
  }

  present(READY_CHILD_ID, S_BINARY);
  present(WORKING_CHILD_ID, S_BINARY);
  present(FINISHED_CHILD_ID, S_BINARY);
}


void loop()      
{ 
  int readySensorVal = digitalRead(READY_PIN);
  if (readySensorVal != readyOldValue) {
     // Send in the new value
     send(msgReady.set(readySensorVal==HIGH ? 1 : 0));
     readyOldValue = readySensorVal;
  }
  int workingSensorVal = digitalRead(WORKING_PIN);
  if (workingSensorVal != workingOldValue) {
     // Send in the new value
     send(msgWorking.set(workingSensorVal==HIGH ? 1 : 0));
     workingOldValue = workingSensorVal;
  }
  int finishedSensorVal = digitalRead(FINISHED_PIN);
  if (finishedSensorVal != finishedOldValue) {
     // Send in the new value
     send(msgFinished.set(finishedSensorVal==HIGH ? 1 : 0));
     finishedOldValue = finishedSensorVal;
  }

  
}

void receive(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
  if (message.type==V_STATUS) {
     // Change relay state
     digitalWrite(message.sensor-1+RELAY_1, message.getBool()?RELAY_OFF:RELAY_ON);
     // Store state in eeprom - saving state is not necessary
     // saveState(message.sensor, message.getBool());
     // Write some debug info
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());
   } 
}
