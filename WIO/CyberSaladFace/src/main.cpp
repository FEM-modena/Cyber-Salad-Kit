/**
 * CyberSaladFace - UI FrontEnd for Arduino CyberSalad
 * Future Education Modena - 2023
 * 
 * Implements a cyclic display mode and a cyclic slave interaction with a serial master
 * 
 * Display Modes are: Face and Graph at cyclic rotation + Message with priority
 * Slave interaction is by listening on Serial communication and putting commands in a command queue 
 */

#include <Arduino.h>
#include "SerialCommands.h"
#include "Visualization.h"

#define LOOP_DELAY 3000

SerialCommands serialCommands;
static std::string charSeq = "";

Visualization curr_visualization;

void actuate(command_spec_t cmd);

void setup() {
  //Setup seriale
  //Serial1.begin(115200);  
  SerialUSB.begin(115200);  
  while (!SerialUSB);
}

void loop() {
  //Check arrival of complete commands from serial
  charSeq.clear();
  while (SerialUSB.available() > 0) {
    char c = SerialUSB.read();
    charSeq.append(1, c);
  }
  if (charSeq.size() > 0) serialCommands.AddString(charSeq);
  //Parse command
  command_spec_t cmd = serialCommands.NextCommand();
  //Apply command by changing mode
  if (cmd.command != CMD_NOTHING) actuate(cmd);
  //Implement visualization based on current mode in currentVisualization
  curr_visualization.Update();

  delay(LOOP_DELAY);
}

void actuate(command_spec_t cmd) {

}

