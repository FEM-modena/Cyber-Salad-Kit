/**
 * CyberSaladFace - UI FrontEnd for Arduino CyberSalad
 * Future Education Modena - 2023
 * 
 * The Face handles the display of a nice Face on the graphic port
 * 
 */

#ifndef FACE_H
#define FACE_H

#include <Arduino.h>

class Face {
public:
	Face();
	void Smile();
    void Sad();
    void Curious();

	//virtual ~Face();
    
};

#endif