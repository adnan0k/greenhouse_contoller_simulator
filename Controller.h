/*
    * Controller.h
    * Author: Wania Amir
    * Created on: 2024-06-01
*/ 

#include <string.h>
#include <cstdint>
using namespace std;

// State Definitions
const int IDLE = 0;
const int IRRIGATION = 1;
const int CRITICAL_CLIMATE = 2;
const int EMERGENCY_STOP = 3;

struct EnvironmentalData {
    uint16_t Data; 
};

struct ControllerOutputs {
    bool pumpOn;
    bool fanOn;
    bool heaterOn;
    bool emergencyStatus;
};

class Controller {
private:
    int state;

public:
    Controller () {
        state = IDLE;
    }

    ControllerOutputs systemController(EnvironmentalData data) {
        ControllerOutputs outputs = {false, false, false, false};

        return outputs;
    }
};