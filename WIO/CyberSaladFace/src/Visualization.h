/**
 * CyberSaladFace - UI FrontEnd for Arduino CyberSalad
 * Future Education Modena - 2023
 * 
 * The Visualization implements the visualization logic
 */

#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <Arduino.h>

class Visualization {
public:
	Visualization();
    void Update();
	//virtual ~Visualization();
private:
  bool faceEnabled;
  bool graphEnabled;
  bool graphNWEnabled;
  bool graphNEEnabled;
  bool graphSEEnabled;
  bool graphSWEnabled;
  bool messageEnabled;
  int messageVisCounter;    
};

#endif