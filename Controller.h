/*
    * Controller.h
    * Author: Wania Amir
    * Created on: 2024-06-01
*/ 

#include <string>
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
        
        // Environment Data Extraction
        // Check for error (bit 15)
        uint16_t checkError = (data.Data & 0x8000) >> 15;
        if (checkError) {
            state = EMERGENCY_STOP;
        }

        int moisture = (data.Data >> 0) & 0xF; // Bits 0-3
        int temperature = (data.Data >> 4) & 0x7F; // Bits 4-10
        int sunlight = (data.Data >> 11) & 0x1FFF; // Bits 11-24

        return outputs;
    }

    string getState() {
        switch (state) {
            case IDLE:
                return "Idle";
            case IRRIGATION:
                return "Irrigation";
            case CRITICAL_CLIMATE:
                return "Critical Climate";
            case EMERGENCY_STOP:
                return "Emergency Stop";
            default:
                return "Unknown State";
        }
    }
};