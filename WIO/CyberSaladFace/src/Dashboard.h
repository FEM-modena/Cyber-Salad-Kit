/**
 * CyberSaladFace - UI FrontEnd for Arduino CyberSalad
 * Future Education Modena - 2023
 * 
 * The Dashboard implements a 4 port graph display of time series
 */

#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <Arduino.h>

class Dashboard {
public:
	Dashboard();
	void Init();
    void DefPort(int id);
    void ShowPort(bool show, int id);
    void AddData(float data, int id);
    void HideAll();
    void ShowAll();
	//virtual ~Dashboard();
    
};

#endif