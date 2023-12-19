/**
 * CyberSaladFace - UI FrontEnd for Arduino CyberSalad
 * Future Education Modena - 2023
 * 
 * The message Display handles messages displayed on the WIO graphic port
 * 
 */

#ifndef MESSAGEDISPLAY_H
#define MESSAGEDISPLAY_H

#include <Arduino.h>
#include <string>

class MessageDisplay {
public:
	MessageDisplay();
	void MessageToShow(std::string);
    void ShowOnce();
    void ShowFixed();
    void HideFixed();
	//virtual ~MessageDisplay();
    
};

#endif